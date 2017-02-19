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

  std::pair<long, bool> CNFFormula::AssignmentWeight(
      const VariableAssignment& ass) const {
    auto weight_and_num = std::accumulate(std::cbegin(clauses),
                           std::cend(clauses),
                           std::make_pair<long, int>(0L, 0),
                           [&] (auto&& acc, const CNFClause& clause) {
                                if (check_clause(clause, ass)) {
                                   acc.first+=clause_weight(clause);
                                   ++acc.second;
                                   return acc;
                                } else return acc;
                           });
    return {weight_and_num.first, weight_and_num.second == clauses.size()};
  }
  
  using std::experimental::optional;

  namespace {
    bool pick_variable(const double& number, int num_vars) {
      auto picking_variable = util::get_int_flag("picking_variable");
      if (picking_variable > 0) {
          return number < double(picking_variable) / double(num_vars); 
      }
      return number < 0.5; 
    }

    CNFClause random_CNS_clause(int num_vars, optional<int> seed) {
      CNFClause result;
     do { 
      for (int var_id = 0; var_id < num_vars; ++var_id) {
        // Roll a dice to decide if to take this variable.
        if (pick_variable(util::random_real(seed), num_vars)) {
          // Roll a dice to decide if to take this variable or its complement.
          result.vars.emplace_back(var_id, util::random_real(seed) < 0.5);
        }
      }
     } while (result.vars.size() == 0);
      return result;
    }

  }  // namespace
  std::vector<CNFClause> build_random_clauses(int num_vars, int num_clauses,
      optional<int> seed) {
    std::vector<CNFClause> result(num_clauses);
    std::generate(std::begin(result), std::end(result),
        [&num_vars, seed] () {return random_CNS_clause(num_vars, seed);});
    return result;
  }

  std::ostream& operator<<(std::ostream& os, const CNFLiteral& v) {
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
