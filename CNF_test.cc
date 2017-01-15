#include "CNF.h"
#include "local-search.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <thread>

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
  util::add_int_flag("switch_brute_force", "when to switch to brute force ", 5);
  util::add_string_flag("experiments_file", "the file to use for creating"
      "reports", "out/experiments.json");
  util::add_bool_flag("create_experiments_file", "run create_experiments_file",
      false);
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

void create_experiments_file() {
  std::vector<util::experiment_info> experiments(500);
  int num_vars = 2;
  int num_clauses = util::get_int_flag("n_clauses");;

  for (auto& exp : experiments) {
    exp.num_vars = num_vars;
    exp.num_clauses = num_clauses;
    exp.how_many_runs = 100;
    ++num_vars;
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

int main(int argc, char** argv) {
  init(argc, argv);
  auto local_search =
    [num_steps = util::get_int_flag("local_steps")] (const auto& formula) { 
      return LocalSearch(formula, num_steps);};
  auto cond_expect = [] (const auto& f) { return ConditionalExpectations(f);};
  auto brute_solver = [] (const auto& f) { return BruteForce(f);};
  auto cond_opt_to_brute = [] (const auto& f) { return FinishWithBrute(f); };
  if (util::get_bool_flag("create_experiments_file")) {
    create_experiments_file();
  }
  std::thread t1([&] ()
      {CreateSolverJSONReport(cond_expect, "ConditionalExpectation3");});
  std::thread t2 ([&] ()
      {CreateSolverJSONReport(local_search, "LocalSearch3");});
  std::thread t3 ([&] () {
      CreateSolverJSONReport(cond_opt_to_brute, "ConditionalExpectationBrute3");
      });
  t1.join();
  t2.join();
  t3.join();
  return 0;
}
