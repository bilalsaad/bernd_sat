#include "CNF.h"
#include <iostream>

using namespace CNF;
int main(int argc, char** argv) {
  CNFFormula f(argc > 1 ? std::stoi(*(argv+1)) : 10,
               argc > 2 ? std::stoi(*(argv+2)) : 5);
  std::cout << f;

  auto ass = RandomVariableAssignment(f.NumVars());
  PrintAssignment(std::cout, ass, f.NumVars());
  std::cout << f.AssignmentWeight(ass) << std::endl;
  return 0;
}
