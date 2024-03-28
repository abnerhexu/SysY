#include "codegen.h"
namespace codegen {
  class LLIRGen {
public:
  sysy::Module* module;
  int bblabelId = 0;
  sysy::Function* curFunc;
  sysy::BasicBlock* curBBlock;

public:
  void llir_gen();
  void module_gen(sysy::Module* module);
  void clearModuleLabels(sysy::Module *module) {
    this->bblabelId = 0;
  }
  void function_gen_Pass1(sysy::Function *func);
  void function_gen_Pass2(sysy::Function *func);
  void clearFuncInfo(sysy::Function *func) {
    //TODO this->OffsetAcc = 0;
    ;
  }
  void basicBlock_gen(sysy::BasicBlock *bb);
  void instruction_gen(sysy::Instruction *inst);
  void GenBinaryInst(sysy::BinaryInst *inst);
  void GenAllocaInst(sysy::AllocaInst *inst);
  void GenStoreInst(sysy::StoreInst *inst);
  void GenLoadInst(sysy::LoadInst *inst);
  }; // class LLIR gen
}