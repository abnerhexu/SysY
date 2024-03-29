#include "llir.h"

namespace codegen {
void LLIRGen::llir_gen() {
    this->module_gen(this->module);
    this->curReg = 32;
    return;
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
    int cnt = function_gen_Pass1(func); //Number of Alloca instructions
    function_gen_Pass2(func);
  }
}

int LLIRGen::function_gen_Pass1(sysy::Function* func) {
  //TODO
  int cnt = 0;
  this->curFunc = func;
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    for (auto &inst: bb->getInstructions()) {
      //it->print(std::cout);
      if (inst->getKind() == sysy::Value::Kind::kAlloca){
        cnt++;
      }
    }
  }
  //std::cout << func->getName() << ' ' << cnt << std::endl;
  return cnt;
}

void LLIRGen::function_gen_Pass2(sysy::Function* func) {
  this->curFunc = func;
  int alloca_index = 0;
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    alloca_index = basicBlock_gen(bb, alloca_index);
  }
}

int LLIRGen::basicBlock_gen(sysy::BasicBlock* bb, int alloca_index) {
  this->curBBlock = bb;
  int tot_alloca = alloca_index;
  for (auto &inst: bb->getInstructions()) {
    // auto instType = inst->getKind();
    if (inst->getKind() == sysy::Value::Kind::kAlloca)
      tot_alloca++;
    instruction_gen(inst.get(), tot_alloca);
  }
  return tot_alloca;
}

void LLIRGen::instruction_gen(sysy::Instruction* inst, int alloca_index) {
  auto instType = inst->getKind();
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv: 
      this->GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      this->GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst), alloca_index);
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

void LLIRGen::GenBinaryInst(sysy::BinaryInst *inst) {
  //TODO
}
void LLIRGen::GenAllocaInst(sysy::AllocaInst *inst, int alloca_index) {
  //TODO
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, 4*alloca_index}});
}
void LLIRGen::GenStoreInst(sysy::StoreInst *inst) {
  //TODO
}
void LLIRGen::GenLoadInst(sysy::LoadInst *inst) {
  //TODO
  //std::cout << inst->getName() << std::endl;
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::InReg, this->curReg}});
  this->curReg++;
}
} // namespace codegen