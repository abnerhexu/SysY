#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "SysYLexer.h"
#include "SysYParser.h"
#include "frontend/SysYIRGenerator.h"
#include "debug.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << "inputfile\n";
    return EXIT_FAILURE;
  }
  std::ifstream fin(argv[1]);
  if (not fin) {
    std::cerr << "Failed to open file " << argv[1];
    return EXIT_FAILURE;
  }
  antlr4::ANTLRInputStream input(fin);
  SysYLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  auto module = parser.module();

  sysy::SysYIRGenerator generator;
  generator.visitModule(module);

  auto moduleIR = generator.get();
  moduleIR->print(std::cout);

  return EXIT_SUCCESS;
}