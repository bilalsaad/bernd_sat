#include "CNF.h"
#include "util.h"
#include <algorithm>
#include <numeric>
#include <ostream>
namespace CNF {

  namespace {
    bool check_clause(const CNFClause& clause,
                      const VariableAssignment& ass) {
      for (const auto& v : clause.vars) {
        if (ass[v.id] != v.is_complement) {
          return true;
        }
      }
      return false;
    }
  }  // namespace

  long CNFFormula::AssignmentWeight(const VariableAssignment& ass) {
    return std::accumulate(std::cbegin(clauses),
                           std::cend(clauses),
                           0L,
                           [&] (auto&& acc, const CNFClause& clause) {
                                if (check_clause(clause, ass)) {
                                   return acc += clause_weight(clause);
                                } else return acc;
                           });
  }

  namespace {
    CNFClause random_CNS_clause(int num_vars) {
      CNFClause result;
      for (int var_id = 0; var_id < num_vars; ++var_id) {
        // Roll a dice to decide if to take this variable.
        if (util::random_real() < 0.5) {
          // Roll a dice to decide if to take this variable or its complement.
          result.vars.emplace_back(var_id, util::random_real() < 0.5);
        }
      }
      return result;
    }

  }  // namespace
  std::vector<CNFClause> build_random_clauses(int num_vars, int num_clauses) {
    std::vector<CNFClause> result(num_clauses);
    std::generate(std::begin(result), std::end(result),
        [&num_vars] () {return random_CNS_clause(num_vars);});
    return result;
  }

  std::ostream& operator<<(std::ostream& os, const CNFVariable& v) {
    os << "[" << (v.is_complement ? "!" : "") << "x_" << v.id << "]";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const CNFClause& c) {
    os << "{";
    for (const auto& v : c.vars) {
      os << v << " ";
    } 
    os << "}";
    return os;
  }
  std::ostream& operator<<(std::ostream& os, const CNFFormula& cnf) {
    for (const auto& c : cnf.clauses) {
      os << c << "\n";
    }
    return os;
  }

  VariableAssignment RandomVariableAssignment(int k) {
    std::vector<bool> assignment(k);
    for (int i = 0; i < k; ++i) {
      assignment[i] = util::random_real() < 0.5; 
    }
    return assignment;
  }
  std::ostream& PrintAssignment(std::ostream& os, const VariableAssignment& ass,
      int num_vars) {
    os << "Assignment is:\n";
    for (int i = 0; i < num_vars; ++i) {
      os << "[" << (ass[i] ? "" : "!") << "x_" << i << "] ";
    }
    os << std::endl;

    return os;
  }

} // namespace CNF
