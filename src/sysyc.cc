#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "SysYLexer.h"
#include "SysYParser.h"
#include "frontend/SysYIRGenerator.h"
#include "backend/codegen.h"
#include "backend/llir.h"
#include "backend/assembly.h"
#include "opt/peephole.h"

struct ArgsOptions {
  std::string srcfile;
  bool emitIr = false;
  bool emitAs = false;
  bool constPropagation = false;
  bool outputBinary = false;
};

class TransformFlags {
public:
  sysy::Module *module;
  TransformFlags(sysy::Module* module): module(module) {};
  std::map<std::string, bool> flags = {{"--emit-ir", false}, 
                                       {"--emit-as", false},
                                       {"--const-propagation", false},
                                       {"--output-binary", false},
                                       {"--fpeephole", false},
                                       {"--verbose", false},
                                       {"--fparallel", false},
                                       {"--opt", false}};
  std::string srcfile;
  void parseFlags(int argc, char** argv) {
    this->srcfile = argv[1];
    for (int i = 2; i < argc; i++) {
      std::string arg = argv[i];
      if (flags.find(arg) != flags.end()) {
        flags[arg] = true;
      }
    }
    if (this->flags["--verbose"]) {
      this->flags["--emit-ir"] = true;
      this->flags["--emit-as"] = true;
      this->flags["output-binary"] = true;
    }
    if (this->flags["--opt"]) {
      this->flags["--emit-ir"] = true;
      this->flags["--emit-as"] = true;
      this->flags["--const-propagation"] = true;
      this->flags["--fpeephole"] = true;
      this->flags["--fparallel"] = true;
    }
  }
  void transformationPending() {
    if (this->flags["--fpeephole"]) {
      transform::Hole hole(this->module);
      hole.moduleTransform();
    }
  }
  void emit() {
    if (this->flags["--emit-ir"]) {
      this->module->print(std::cout);
    }
    if (this->flags["--emit-as"]) {
      auto foutName = this->srcfile.erase(this->srcfile.find_last_of("."), this->srcfile.size()).append(".s");
      std::cout << "assembly code output: " << foutName << std::endl;
      std::ofstream fout(foutName);
      codegen::AssemblyCode assemblyCode(this->module);
      assemblyCode.emitModule(fout);
      fout.close();
    }
    if (this->flags["--output-binary"]) {
      
    }
  }
}; // class TransformFlags

int main(int argc, char *argv[]) {
  TransformFlags tff(nullptr);
  tff.parseFlags(argc, argv);
  std::ifstream fin(tff.srcfile);
  if (not fin) {
    std::cerr << "Failed to open file " << argv[1];
    return EXIT_FAILURE;
  }
  antlr4::ANTLRInputStream input(fin);
  fin.close();
  SysYLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  auto module = parser.module();

  sysy::SysYIRGenerator generator;
  generator.visitModule(module);

  auto moduleIR = generator.get();
  tff.module = moduleIR;
  // moduleIR->print(std::cout);
  codegen::LLIRGen llirgenerator(moduleIR);
  llirgenerator.llir_gen();

  codegen::CodeGen codeGenerator(moduleIR);
  codeGenerator.code_gen();

  tff.transformationPending();
  tff.emit();
  return EXIT_SUCCESS;
}