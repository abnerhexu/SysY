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
    function_gen_Pass1(func); //Number of Alloca instructions
    function_gen_Pass2(func);
  }
}

void LLIRGen::function_gen_Pass1(sysy::Function* func) {
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
  regManager.spOffset.insert({func, cnt*4}); //TODO array support needed
  //std::cout << func->getName() << ' ' << cnt << std::endl;
}

void LLIRGen::function_gen_Pass2(sysy::Function* func) {
  this->curFunc = func;
  int alloca_offset = 0;
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    alloca_offset = basicBlock_gen(bb, alloca_offset);
  }
}

int LLIRGen::basicBlock_gen(sysy::BasicBlock* bb, int alloca_offset) {
  this->curBBlock = bb;
  int tot_offset = alloca_offset;
  for (auto &inst: bb->getInstructions()) {
    // auto instType = inst->getKind();
    if (inst->getKind() == sysy::Value::Kind::kAlloca){
      auto allocateType = static_cast<const sysy::PointerType*>(inst->getType())->getBaseType();
      if (allocateType->isInt()){
        tot_offset += 4;
      }
      else if (allocateType->isFloat()) {
        tot_offset += 4;
      }
      else {
        std::cerr << static_cast<const sysy::PointerType*>(inst->getType())->getBaseType()->getKind() << std::endl;
        std::cerr << "Unsupported type in alloca" << std::endl;
        exit(1);
      }
    }
    instruction_gen(inst.get(), tot_offset);
  }
  return tot_offset;
}

void LLIRGen::instruction_gen(sysy::Instruction* inst, int alloca_offset) {
  auto instType = inst->getKind();
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv: 
      this->GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      this->GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst), alloca_offset);
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
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::InReg, this->curReg}});
  this->curReg++;
}
void LLIRGen::GenAllocaInst(sysy::AllocaInst *inst, int alloca_offset) {
  //TODO
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, alloca_offset}});
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