#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "SysYLexer.h"
#include "SysYParser.h"
#include "frontend/SysYIRGenerator.h"
#include "debug.h"
#include "backend/codegen.h"
#include "backend/llir.h"
#include "backend/assembly.h"

struct ArgsOptions {
  std::string srcfile;
  bool emitIr = false;
  bool emitAs = false;
  bool constPropagation = false;
  bool outputBinary = false;
};

ArgsOptions parseArguments(int argc, char* argv[]) {
  ArgsOptions options;
  options.srcfile = argv[1];
  for(int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--emit-ir") {
      options.emitIr = true;
    } 
    else if (arg == "--emit-as") {
      options.emitAs = true;
    } 
    else if (arg == "--const-propagation") {
      options.constPropagation = true;
    } 
    else if (arg == "--output-binary") {
      options.outputBinary = true;
    }
    else if (arg == "--verbose") {
      options.emitIr = true;
      options.emitAs = true;
    } 
    else {
      std::cerr << "Unknown argument: " << arg << std::endl;
    }
  }
  return options;
}


int main(int argc, char *argv[]) {
  ArgsOptions args = parseArguments(argc, argv);
  std::ifstream fin(args.srcfile);
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
  if (args.emitIr) {
    moduleIR->print(std::cout);
  }
  
  codegen::LLIRGen llirgenerator(moduleIR);
  llirgenerator.llir_gen();

  codegen::CodeGen codeGenerator(moduleIR);
  codeGenerator.code_gen();

  if (args.emitAs) {
    codegen::AssemblyCode assemblyCode(moduleIR);
    assemblyCode.emitModule(std::cout);
  }
  return EXIT_SUCCESS;
}