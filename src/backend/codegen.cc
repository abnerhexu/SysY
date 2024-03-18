#include <iostream>
#include "codegen.h"

namespace codegen {

std::string CodeGen::code_gen() {
  std::string assemblyCode;
  assemblyCode += this->module_gen(module);
  return assemblyCode;
}

std::string CodeGen::module_gen(sysy::Module *module) {
  std::string descriptionCode;
  std::string dataCode;
  std::string textCode;
  std::string compilerIdentifier = "SysY compiler";

  clearModuleLabels(module);

  descriptionCode += space + ".file" + this->fname + endl;
  descriptionCode += space + ".attribute risc-v rv64gc little-endian" + endl;
  descriptionCode += space + ".text" + endl;
  descriptionCode += space + ".globl main" + endl;
  dataCode += globalData_gen(); 
  
   
}

}