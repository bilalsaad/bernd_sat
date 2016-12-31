#ifndef __LOCAL_SEARCH_H_
#define __LOCAL_SEARCH_H_
#include "CNF.h"
#include "util.h"

namespace CNF {
VariableAssignment LocalSearch(const CNFFormula& cnf_formula, int max_steps);
}  // namespace CNF
#endif
