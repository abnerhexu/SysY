#include "llir.h"
#include "../frontend/SysYIRGenerator.h"

namespace codegen {

void LLIRGen::llir_gen() {
    this->module_gen(this->module);
    return;
}

void LLIRGen::module_gen(sysy::Module* module) {
  this->clearModuleLabels(module);
  // this->globalData_gen(module); // This function has been moved to the backend
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
  // Pass 1: get the sp off
  int cnt = 0;
  this->curFunc = func;
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    for (auto &inst: bb->getInstructions()) {
      //it->print(std::cout);
      if (inst->getKind() == sysy::Value::Kind::kAlloca){
        int value_size = 1;
        auto inst_release = inst.get(); // inst.release();
        for (int i = 0; i < dynamic_cast<sysy::AllocaInst*>(inst_release)->getNumDims(); i++){
          // std::cout << dynamic_cast<sysy::ConstantValue*>(dynamic_cast<sysy::AllocaInst*>(inst_release)->getDim(i))->getInt() << std::endl;
          value_size *= dynamic_cast<sysy::ConstantValue*>(dynamic_cast<sysy::AllocaInst*>(inst_release)->getDim(i))->getInt();
        }
        cnt += value_size;
      }
    }
  }
  regManager.spOffset.insert({func, cnt*4});
  //std::cout << func->getName() << ' ' << cnt << std::endl;
}

void LLIRGen::function_gen_Pass2(sysy::Function* func) {
  // Pass 2: map the vars to offs
  this->curFunc = func;
  int alloca_offset = 4-regManager.spOffset[func];
  clearFuncInfo(func);
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    alloca_offset = basicBlock_gen(bb, alloca_offset);
  }
  // for (auto &t : this->LastVisit){
  //   std::cout << t.first << ' ' << t.second << std::endl;
  // }
}

int LLIRGen::basicBlock_gen(sysy::BasicBlock* bb, int alloca_offset) {
  this->curBBlock = bb;
  int tot_offset = alloca_offset;
  for (auto &inst: bb->getInstructions()) {
    instruction_gen(inst.get(), tot_offset);
    if (inst->getKind() == sysy::Value::Kind::kAlloca){
      auto allocateType = static_cast<const sysy::PointerType*>(inst->getType())->getBaseType();
      if (allocateType->isInt()){
        if (dynamic_cast<sysy::AllocaInst*>(inst.get())->getNumDims()){
          int arr_size = 1;
          auto shape = sysy::usedarrays.find(dynamic_cast<sysy::AllocaInst*>(inst.get())->getName());
          for (int i = 0; i < dynamic_cast<sysy::AllocaInst*>(inst.get())->getNumDims(); i++){
            std::cout << dynamic_cast<sysy::ConstantValue*>(dynamic_cast<sysy::AllocaInst*>(inst.get())->getDim(i))->getInt() << std::endl;
            arr_size *= dynamic_cast<sysy::ConstantValue*>(dynamic_cast<sysy::AllocaInst*>(inst.get())->getDim(i))->getInt();
          }
          tot_offset += 4*arr_size;
        }else{
          tot_offset += 4;
        }
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
  }
  return tot_offset;
}

void LLIRGen::instruction_gen(sysy::Instruction* inst, int alloca_offset) {
  auto instType = inst->getKind();
  this->inst_index += 1;
  // std::cout << inst->getName() << std::endl;
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv:
    case sysy::Value::Kind::kRem:
    case sysy::Value::Kind::kICmpEQ:
    case sysy::Value::Kind::kICmpNE:
    case sysy::Value::Kind::kICmpLT:
    case sysy::Value::Kind::kICmpGT:
    case sysy::Value::Kind::kICmpLE:
    case sysy::Value::Kind::kICmpGE:
    case sysy::Value::Kind::kFAdd:
    case sysy::Value::Kind::kFSub:
    case sysy::Value::Kind::kFMul:
    case sysy::Value::Kind::kFDiv:
    case sysy::Value::Kind::kFRem:
    case sysy::Value::Kind::kFCmpEQ:
    case sysy::Value::Kind::kFCmpNE:
    case sysy::Value::Kind::kFCmpLT:
    case sysy::Value::Kind::kFCmpGT:
    case sysy::Value::Kind::kFCmpLE:
    case sysy::Value::Kind::kFCmpGE:
      this->GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      this->GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst), alloca_offset);
      // std::cout << inst->getName() << std::endl;
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
  inst->inst_index = this->inst_index;
  if (inst->getNumOperands() != 0){
    for (int i = 0; i < inst->getNumOperands(); i++){
      if (regManager.varIRegMap.find(inst->getOperand(i)->getName()) != regManager.varIRegMap.end() && 
          regManager.varIRegMap[inst->getOperand(i)->getName()].first == RegisterManager::VarPos::InIReg) {
        int llir_reg = regManager.varIRegMap[inst->getOperand(i)->getName()].second;
        regManager.LastVisit[llir_reg].first = this->inst_index;
        regManager.LastVisit[llir_reg].second->last_used = this->inst_index;
      }
      if (regManager.varFRegMap.find(inst->getOperand(i)->getName()) != regManager.varIRegMap.end() &&
          regManager.varFRegMap[inst->getOperand(i)->getName()].first == RegisterManager::VarPos::InFReg) {
        int llir_reg = regManager.varIRegMap[inst->getOperand(i)->getName()].second;
        regManager.LastVisit[llir_reg].first = this->inst_index;
        regManager.LastVisit[llir_reg].second->last_used = this->inst_index;
      }
    }
  }
}

void LLIRGen::GenBinaryInst(sysy::BinaryInst *inst) {
  if (inst->getType()->isInt()) {
    regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::InIReg, this->curReg}});
  }
  else {
    regManager.varFRegMap.insert({inst->getName(), {RegisterManager::VarPos::InFReg, this->curReg}});
  }
  regManager.LastVisit.insert({this->curReg, {this->inst_index, inst}});
  this->curReg++;
}
void LLIRGen::GenAllocaInst(sysy::AllocaInst *inst, int alloca_offset) {
  if (dynamic_cast<sysy::PointerType*>(inst->getType())->getBaseType()->isInt()) {
    regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, alloca_offset}});
  }
  else {
    regManager.varFRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, alloca_offset}});
  }
}
void LLIRGen::GenStoreInst(sysy::StoreInst *inst) {
  //TODO
}
void LLIRGen::GenLoadInst(sysy::LoadInst *inst) {
  //std::cout << inst->getName() << std::endl;
  if (inst->getType()->isInt()) {
    regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::InIReg, this->curReg}});
  }
  else {
    regManager.varFRegMap.insert({inst->getName(), {RegisterManager::VarPos::InFReg, this->curReg}});
  }
  regManager.LastVisit.insert({this->curReg, {this->inst_index, inst}});
  this->curReg++;
}
} // namespace codegen