#include "local-search.h"

namespace CNF {

namespace {
  void flip_variable(VariableAssignment& ass) {
    std::srand(std::time(0));
    int random_index = std::rand() % ass.size();
    ass[random_index] = !ass[random_index];
  }
}  // namespace
  VariableAssignment LocalSearch(const CNFFormula& cnf_formula, int max_steps) {
    auto result = RandomVariableAssignment(cnf_formula.NumVars());
    //VariableAssignment result(cnf_formula.NumVars(), false);
    auto temp_result = result;
    auto best_val = cnf_formula.AssignmentWeight(result);
    auto first_score = best_val.first;
    for (int i = 0; i < max_steps; ++i) {
      auto temp_val = cnf_formula.AssignmentWeight(temp_result);
      if (temp_val.first > best_val.first) {
        result = temp_result;
        best_val = temp_val;
      }
      if (temp_val.second == true) {
        break;
      }
      flip_variable(temp_result);
    }
    auto final_score = best_val.first;
    std::cout << "First score: " << first_score << std::endl;
    std::cout << "Final score: " << final_score << std::endl;
    return result;
  }

namespace {

  VariableAssignment brute_force_helper(VariableAssignment& ass,
      const CNFFormula& cnf_formula, int i) {
    if (i == cnf_formula.NumVars()) {
      return ass;
    }
    auto ass_i_true = ass;
    ass_i_true[i] = true;
    ass_i_true = brute_force_helper(ass_i_true, cnf_formula, i + 1);
    auto ass_i_false = ass;
    ass_i_false[i] = false;
    ass_i_false = brute_force_helper(ass_i_false, cnf_formula, i + 1);
    return cnf_formula.AssignmentWeight(ass_i_true).first >
           cnf_formula.AssignmentWeight(ass_i_false).first ?
           ass_i_true : ass_i_false;
  }


  VariableAssignment BruteForce(const CNFFormula& cnf_formula) {
    VariableAssignment res;
    brute_force_helper(res, cnf_formula, 0);
    return res;
  }
}  // namespace


}  // namespace CNF
