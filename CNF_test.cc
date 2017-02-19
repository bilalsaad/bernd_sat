#include "CNF.h"
#include "local-search.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <future>

using namespace CNF;
using std::experimental::optional;
using json = nlohmann::json;
void init(int argc, char** argv) {
  util::add_int_flag("n_clauses", "number of CNF clauses", 5);
  util::add_int_flag("n_vars", "number of CNF variables", 10);
  util::add_int_flag("local_steps", "steps for local search ", 50);
  util::add_bool_flag("verbose", "print the CNF formula", false);
  util::add_int_flag("seed", "a random seed to reproduce cnf formulas", 1337);
  util::add_bool_flag("use_seed", "use seed ", false);
  util::add_int_flag("n_test_runs" , "how many test runs", 10000);
  util::add_int_flag("switch_brute_force", "when to switch to brute force ", 7);
  util::add_string_flag("experiments_file", "the file to use for creating"
      "reports", "out/experiments.json");
  util::add_bool_flag("create_experiments_file", "run create_experiments_file",
      false);
  util::add_string_flag("run_config", "what to run", "in/config.json");
  util::add_string_flag("report_suffix" ,
      "suffix of reports created by this run",
      "300");
  util::add_int_flag("picking_variable" , "how to pick variables "
      " if less then 1 we pick with probability 1/2 else picking_variable/n "
      "where n is the number of variables in the cnf clause", 3);
  util::parse_flags(argc, argv);
}

template<typename SatSolverA, typename SatSolverB>
auto compare_sat_solvers(SatSolverA&& A, SatSolverB&& B,
    const CNFFormula& cnf_formula) {
  auto assignment_A = A(cnf_formula);
  auto assignment_B = B(cnf_formula);
  return cnf_formula.AssignmentWeight(assignment_A).first -
    cnf_formula.AssignmentWeight(assignment_B).first; 
}

template<typename SatSolverA, typename SatSolverB >
void CompareSolvers(SatSolverA&& A, SatSolverB&& B) {
  double running_difference = 0;
  int num_runs = util::get_int_flag("n_test_runs");
  for (int i = 0; i < num_runs; ++i) {
    CNFFormula f(util::get_int_flag("n_vars"), util::get_int_flag("n_clauses"));
    running_difference += compare_sat_solvers(A, B, f);
  }
  std::cout << "Average difference over " << num_runs << " runs is " <<
    running_difference / double(num_runs) << std::endl;
}

void create_experiments_file(int var_inc = 2) {
  std::vector<util::experiment_info> experiments(50);
  int num_vars = 100;
  int num_clauses = util::get_int_flag("n_clauses");;

  for (auto& exp : experiments) {
    exp.num_vars = num_vars;
    exp.num_clauses = num_clauses;
    exp.how_many_runs = 100;
    num_vars += var_inc;
  }
  std::vector<json> jsons;
  std::transform(std::begin(experiments), std::end(experiments),
      std::back_inserter(jsons), [] (auto&& e) -> json { return e; });
  json b(jsons);
  std::ofstream o(util::get_string_flag("experiments_file"));
  o << std::setw(4) << b << std::endl;
}


template<typename Solver>
auto RunExperiment(Solver&& solver, const util::experiment_info& info) {
  long sum = 0L;
  for (int i = 0; i < info.how_many_runs; ++i) {
    CNFFormula f(info.num_vars, info.num_clauses);
    sum += f.AssignmentWeight(solver(f)).first;
  } 
  return double(sum) / double(info.how_many_runs);
}

template<typename Solver>
void CreateSolverJSONReport(Solver&& A, const std::string& report_prefix) {
  auto experiment_json_result_json = [&] (auto&& exp) -> json {
      util::experiment_info info (exp);
      auto result = RunExperiment(A, info);
      json result_json;
      result_json["num_vars"] = info.num_vars;
      result_json["num_clauses"] = info.num_clauses;
      result_json["average_result"] = result;
      return result_json;
  };
  std::ifstream experiments(util::get_string_flag("experiments_file"));
  json j;
  experiments >> j;
  std::ofstream output_file("out/" + report_prefix + "solver_report.json");

  output_file << "[" << std::endl;
  output_file << std::setw(4) << experiment_json_result_json(j[0]);
  if (j.size() != 0) {
   for (auto iter = std::begin(j) + 1; iter != std::end(j); ++iter) {
      auto&& experiment = *iter;
      // The average of running solver A on the parameters in experiment.
      output_file << "," << std::endl <<
        std::setw(4) << experiment_json_result_json(*iter) ;
   } 
  }
  output_file << "]" << std::endl;
}

json get_config_file(const std::string& fname) {
  std::ifstream experiments(fname);
  json j;
  experiments >> j;
  return j;
}

enum class AlgorithmType {
  LOCAL,
  COND,
  SHORTCIRCUIT,
};

struct ExperimentArgs {
  int num_runs;
  int num_clauses;
  int num_vars;
  std::string alg_type;

  
  // Optional fields for some algorithms.

  // For local search.
  int num_steps = -1;

  // For the short circuiting alg
  int shortcircuit = -1;
 
  ExperimentArgs(const json& args): num_runs(args["num_runs"]),
  num_clauses(args["num_clauses"]), num_vars(args["num_vars"]) {
    num_steps = args.count("num_steps") ? int(args["num_steps"]) : -1;
    shortcircuit = args.count("shortcircuit") ? int(args["shortcircuit"]) : -1;
    alg_type = args["algorithm"]; 
  }
};

template<typename Solver>
json RunExperiment(Solver&& solver, const ExperimentArgs& args) {
  long sum = 0L;
  for (int i = 0; i < args.num_runs; ++i) {
    CNFFormula f(args.num_vars, args.num_clauses);
    sum += f.AssignmentWeight(solver(f)).first;
  } 
  double result = double(sum) / double(args.num_runs);
  json result_json;
  result_json["num_vars"] = args.num_vars;
  result_json["num_clauses"] = args.num_clauses;
  result_json["average_result"] = result;
  result_json["algorithm"] = args.alg_type;
  return result_json;
}

class Experimentor {
  public:
    json operator()(const ExperimentArgs& args) {
      return func(args);
    };
    // Creates an Experimentor object, that once called will conduct on the
    // correct experiment according to 'alg_type' and 'exp_type'.
    static Experimentor CreateExperimentor(AlgorithmType alg_type) {
      static std::unordered_map<AlgorithmType, f_type> func_map =
      { 
        {AlgorithmType::LOCAL,
          [] (const ExperimentArgs& args) -> json { 
            return  RunExperiment([steps = args.num_steps] (auto&& formula) {
                return LocalSearch(formula, steps);}, args);
          }}, 
        {AlgorithmType::COND,
          [] (const ExperimentArgs& args) -> json { 
            return  RunExperiment([](auto&& formula) {
                return ConditionalExpectations(formula);}, args);
          }}, 
        {AlgorithmType::SHORTCIRCUIT,
          [] (const ExperimentArgs& args) -> json { 
            return  RunExperiment([k = args.shortcircuit](auto&& formula) {
                return ConditionalExpectations(formula);}, args);
          }}, 
      };
      return func_map[alg_type];
    }
  private:
    using f_type = std::function<json(const ExperimentArgs& args)>;
    Experimentor(f_type f): func(f) {}
    f_type func;
};

AlgorithmType TypeFromString(const std::string& type) {
  constexpr char kLocal[] = "LocalSearch";
  constexpr char kCond[] = "Cond";
  constexpr char kShortC[] = "ShortCircuit";
  if (type == kLocal)
    return AlgorithmType::LOCAL;
  if (type == kCond)
    return AlgorithmType::COND;
  if (type == kShortC) 
    return AlgorithmType::SHORTCIRCUIT;

  std::cout << "Invalid experiment algorithm must be " << kLocal << " or "
    << kCond << " or " << kShortC;
  assert(false);
}

void ConductExperiments(const json& experiments,
    const std::string& report_suffix) {
  std::vector<std::future<json>> futures(experiments.size());
  for (int i = 0; i < futures.size(); ++i) {
    ExperimentArgs args(experiments[i]);
    auto alg_type = TypeFromString(experiments[i]["algorithm"]);
    auto exp_func = Experimentor::CreateExperimentor(alg_type);
    futures[i] = std::async(exp_func, args);
  }

  std::ofstream outfile(
      "out/Report_" + report_suffix + ".json"); 
  outfile << "[";
  std::string separator = "";
  for (auto& f : futures) {
    auto res = f.get();
    outfile << separator << std::endl << std::setw(4) << res;
    separator = ",";
  }
  outfile << "]";
}

int main(int argc, char** argv) {
  init(argc, argv);
  auto config = get_config_file(util::get_string_flag("run_config"));
  ConductExperiments(config, util::get_string_flag("report_suffix"));
  return 0;
}
