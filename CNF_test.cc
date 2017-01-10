#include "CNF.h"
#include "local-search.h"
#include <iostream>

using namespace CNF;
using std::experimental::optional;

void init(int argc, char** argv) {
  util::add_int_flag("n_clauses", "number of CNF clauses", 5);
  util::add_int_flag("n_vars", "number of CNF variables", 10);
  util::add_int_flag("local_steps", "steps for local search ", 50);
  util::add_bool_flag("verbose", "print the CNF formula", false);
  util::add_int_flag("seed", "a random seed to reproduce cnf formulas", 1337);
  util::add_bool_flag("use_seed", "use seed ", false);
  util::add_int_flag("n_test_runs" , "how many test runs", 10000);
  util::add_int_flag("switch_brute_force", "when to switch to brute force ", 5);
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

int main(int argc, char** argv) {
  init(argc, argv);
  auto local_search =
    [num_steps = util::get_int_flag("local_steps")] (const auto& formula) { 
      return LocalSearch(formula, num_steps);};
  auto cond_expect = [] (const auto& f) { return ConditionalExpectations(f);};
  auto brute_solver = [] (const auto& f) { return BruteForce(f);};
  auto cond_opt_to_brute = [] (const auto& f) { return FinishWithBrute(f); };
  CompareSolvers(cond_expect, local_search);
  return 0;
}
