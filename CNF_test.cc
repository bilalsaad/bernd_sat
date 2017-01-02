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
  util::parse_flags(argc, argv);
}

int main(int argc, char** argv) {
  init(argc, argv);
  CNFFormula f(util::get_int_flag("n_vars"), util::get_int_flag("n_clauses"),
      util::get_bool_flag("use_seed") ?
      optional<int>(util::get_int_flag("seed")) :
        optional<int>());
  auto ass2 = LocalSearch(f, util::get_int_flag("local_steps"));
  auto ass4 = ConditionalExpectations(f);
  if (util::get_bool_flag("verbose")) {
    //std::cout << "cnf formula: \n" << f;
    PrintAssignment(std::cout << "conditional ", ass4, ass4.size());
    PrintAssignment(std::cout << "LocalSearch ", ass2, ass2.size());
  }
  //std::cout << "bruteforce: " << f.AssignmentWeight(ass3).first << std::endl;
  auto local_weight = f.AssignmentWeight(ass2).first;
  auto cond_weight = f.AssignmentWeight(ass4).first;
  std::cout << "Local: " << local_weight << std::endl;
  std::cout << "Conditional: " << cond_weight << std::endl;
  std::cout << "cond - local: " << cond_weight - local_weight << std::endl;
  return 0;
}
