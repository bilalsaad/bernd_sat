#include "CNF.h"
#include "local-search.h"
#include <iostream>

using namespace CNF;
int main(int argc, char** argv) {
  CNFFormula f(argc > 1 ? std::stoi(*(argv+1)) : 10,
               argc > 2 ? std::stoi(*(argv+2)) : 5);
  auto ass = RandomVariableAssignment(f.NumVars());
  auto ass2 = LocalSearch(f, 40);
  return 0;
}
