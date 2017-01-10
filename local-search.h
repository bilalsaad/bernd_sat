#ifndef __LOCAL_SEARCH_H_
#define __LOCAL_SEARCH_H_
#include "CNF.h"
#include "util.h"

namespace CNF {
VariableAssignment LocalSearch(const CNFFormula& cnf_formula, int max_steps,
    bool verbose = false);
VariableAssignment BruteForce(const CNFFormula&);

VariableAssignment ConditionalExpectations(const CNFFormula& cnf_formula);

VariableAssignment FinishWithBrute(const CNFFormula& cnf_formula,
      int switch_to_brute = util::get_int_flag("switch_brute_force"));
}  // namespace CNF
#endif
