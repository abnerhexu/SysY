#include "llir.h"

namespace codegen {
void LLIRGen::llir_gen() {
    this->module_gen(this->module);
}

void LLIRGen::module_gen(sysy::Module* module) {
  this->clearModuleLabels(module);
  //TODO GlobalData handle
  std::map<std::string, sysy::Function*> *funcs = module->getFunctions();
  for (auto it = funcs->begin(); it != funcs->end(); it++) {
    sysy::Function *func = it->second;
    auto bbs = func->getBasicBlocks();
    if (bbs.empty()) {
      continue;
    }
    function_gen_Pass1(func);
    function_gen_Pass2(func);
  }
}

void LLIRGen::function_gen_Pass1(sysy::Function* func) {
  //TODO
}

void LLIRGen::function_gen_Pass2(sysy::Function* func) {
  this->curFunc = func;
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    basicBlock_gen(bb);
  }
}

void LLIRGen::basicBlock_gen(sysy::BasicBlock* bb) {
  this->curBBlock = bb;
  for (auto &inst: bb->getInstructions()) {
    // auto instType = inst->getKind();
    instruction_gen(inst.get());
  }
}

void LLIRGen::instruction_gen(sysy::Instruction* inst) {
  auto instType = inst->getKind();
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv: 
      this->GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      this->GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst));
      break;
    case sysy::Value::Kind::kStore:
      this->GenStoreInst(dynamic_cast<sysy::StoreInst *>(inst));
      break;  
    case sysy::Value::Kind::kLoad:
      this->GenLoadInst(dynamic_cast<sysy::LoadInst *>(inst));
      break;
    default:
      // std::cerr << "Error: unsupported instruction" << std::endl;
      // exit(1);
      break;
  }
}

void GenBinaryInst(sysy::BinaryInst *inst) {
  //TODO
}
void GenAllocaInst(sysy::AllocaInst *inst) {
  //TODO
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, Offset}});
}
void GenStoreInst(sysy::StoreInst *inst) {
  //TODO
}
void GenLoadInst(sysy::LoadInst *inst) {
  //TODO
}
} // namespace codegen