#include "local-search.h"
#include <cmath>

namespace CNF {

  namespace {
    void flip_variable(VariableAssignment& ass) {
      std::srand(std::time(0));
      int random_index = std::rand() % ass.size();
      ass[random_index] = !ass[random_index];
    }
  }  // namespace
  VariableAssignment LocalSearch(const CNFFormula& cnf_formula, int max_steps,
      bool verbose) {
    auto result = RandomVariableAssignment(cnf_formula.NumVars());
    auto temp_result = result;
    auto best_val = cnf_formula.AssignmentWeight(result);
    const auto first_score = best_val.first;
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
    if (verbose) {
      std::cout << "First score: " << first_score << std::endl;
      std::cout << "Final score: " << final_score << std::endl;
    }
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

  }  // namespace
  VariableAssignment BruteForce(const CNFFormula& cnf_formula) {
    VariableAssignment res(cnf_formula.NumVars(), false);
    return brute_force_helper(res, cnf_formula, 0);
  }

  namespace {
    // We want to calculate $E[W| ass].
    // This is the sum over all clauses in the cnf formula f,
    // Sum(j) where each Ci -> 
    double conditional_expectation(const CNFFormula& f,
        const std::vector<bool>& ass) {
      auto prob_clause_satisfied = [&ass] (const auto& clause) -> double {
        // number of variables in clause and not in the partial assignment
        int k = 0;
        // This checks if the clause is satisfied by the partial assignment.
        for (const auto& lit : clause.vars) {
          if (lit.id < ass.size() && ass[lit.id] != lit.is_complement) {
            return 1.0;
          } else if (lit.id >= ass.size()) {
            ++k;
          }
        } 
        // Now we need to compute 1 - (1/2) ^ k 
        return 1.0 - std::pow(0.5, k);
      }; 
      return f.AccumulateClauses(0.0,
          [&f, &prob_clause_satisfied] (double acc, const auto& clause) {
          return acc +=
                 f.GetClauseWeight(clause) * prob_clause_satisfied(clause);
          });
    }
  }  // namespace

  VariableAssignment ConditionalExpectations(const CNFFormula& cnf_formula) {
    VariableAssignment result;
    for (int i = 0; i < cnf_formula.NumVars(); ++i) {
      // try with ith variable true.
      result.push_back(true);
      auto expected_weight_true = conditional_expectation(cnf_formula, result);
      // now try with false.
      result[result.size()-1] = false;
      auto expected_weight_false = conditional_expectation(cnf_formula, result);
      if (expected_weight_true > expected_weight_false)
        result[result.size()-1] = true;
    }
    return result;
  }

  VariableAssignment FinishWithBrute(const CNFFormula& cnf_formula,
      int switch_to_brute) {
    auto brute_index = std::max(0, cnf_formula.NumVars() - switch_to_brute);
    auto result = ConditionalExpectations(cnf_formula);
    return brute_force_helper(result, cnf_formula, brute_index);
  }
}  // namespace CNF
