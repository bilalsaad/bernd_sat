#include <vector>
#include <functional>
#include <ostream>

namespace CNF {
struct CNFVariable {
  bool is_complement;
  int id;
  CNFVariable(int id, bool is_complement): id(id), is_complement(is_complement)
  {}
};
std::ostream& operator<<(std::ostream& os, const CNFVariable& v);

struct CNFClause {
  std::vector<CNFVariable> vars;
};
std::ostream& operator<<(std::ostream& os, const CNFClause& c);

std::vector<CNFClause> build_random_clauses(int num_vars, int num_clauses);

using VariableAssignment = std::vector<bool>;


class CNFFormula {
  public:
    CNFFormula(int num_vars, int num_clauses):num_vars(num_vars),
    clauses(build_random_clauses(num_vars, num_clauses)) {}
    long AssignmentWeight(const VariableAssignment& ass);
    friend std::ostream& operator<<(std::ostream& os, const CNFFormula& cnf);
    int NumVars() {return num_vars;}
  private:
    int num_vars;
    std::vector<CNFClause> clauses;
    std::function<long(const CNFClause& clause)> clause_weight =
      [] (const CNFClause&) {return 1L;};
};

// Returns a random assignment to variables x_1 ... x_k-1
VariableAssignment RandomVariableAssignment(int k);

std::ostream& PrintAssignment(std::ostream& os, const VariableAssignment& ass,
   int num_vars); 
}  // namespace CNF
