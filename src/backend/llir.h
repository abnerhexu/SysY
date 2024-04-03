#include "codegen.h"

namespace codegen {
  class LLIRGen {

friend class CodeGen;

public:
  sysy::Module* module;
  int bblabelId = 0;
  int curReg = 32;
  sysy::Function* curFunc;
  sysy::BasicBlock* curBBlock;
  std::map<int, int> LastVisit;
  int inst_index = 0;

public:
  LLIRGen(sysy::Module* module): module(module) {};
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
  int basicBlock_gen(sysy::BasicBlock *bb, int alloca_index);
  void instruction_gen(sysy::Instruction *inst, int alloca_index);
  void GenBinaryInst(sysy::BinaryInst *inst);
  void GenAllocaInst(sysy::AllocaInst *inst, int alloca_index);

  void GenStoreInst(sysy::StoreInst *inst);
  void GenLoadInst(sysy::LoadInst *inst);
  }; // class LLIR gen
}
