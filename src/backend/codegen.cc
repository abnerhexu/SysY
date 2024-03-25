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
  std::string assemblyCode;

  clearModuleLabels(module);

  descriptionCode += space + ".file" + this->fname + endl;
  descriptionCode += space + ".attribute risc-v rv64gc little-endian" + endl;
  descriptionCode += space + ".text" + endl;
  descriptionCode += space + ".globl main" + endl;
  dataCode += globalData_gen(); 
  std::map<std::string, sysy::Function*> *funcs = module->getFunctions();
  for (auto it = funcs->begin(); it != funcs->end(); it++) {
    std::string fname = it->first;
    sysy::Function *func = it->second;
    auto bbs = func->getBasicBlocks();
    if (bbs.empty()) {
      continue;
    }
    textCode += function_gen(func) + endl;
  }

  assemblyCode += (descriptionCode + dataCode + textCode + endl);
  return assemblyCode;
}

std::string CodeGen::functionHeader_gen(sysy::Function *func){
  std::string assemblyCode;
  assemblyCode += space + ".global " + func->getName() + endl;
  assemblyCode += space + ".type " + func->getName() + ", @function" + endl;
  return assemblyCode;
}

std::string CodeGen::CalleeRegSave_gen(sysy::Function* func){
  std::string assemblyCode;
  return assemblyCode;
}

std::string CodeGen::CalleeRegRestore_gen(sysy::Function* func){
  std::string assemblyCode;
  return assemblyCode;
}

std::string CodeGen::function_gen(sysy::Function *func) {
  this->curFunc = func;
  clearFuncInfo(func);
  std::string bbCode;
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    bbCode = basicBlock_gen(bb);
  }
  std::string assemblyCode;
  std::string headerCode = functionHeader_gen(func);
  std::string calleeRegSaveCode = CalleeRegSave_gen(func);
  std::string calleeRegRestoreCode = CalleeRegRestore_gen(func);

  // TODO literalPoolsCode
  assemblyCode = headerCode + calleeRegSaveCode + bbCode + calleeRegRestoreCode;

  return assemblyCode;
}

std::string CodeGen::basicBlock_gen(sysy::BasicBlock *bb) {
  this->curBBlock = bb;
  std::string bbLabel = this->getBasicBlocksLabel(bb);
  std::string assemblyCode;
  assemblyCode += bbLabel + ":" + endl;
  for (auto &inst: bb->getInstructions()) {
    auto instType = inst->getKind();
    assemblyCode += instruction_gen(inst.get());
  }
  return assemblyCode;
}

std::pair<int, std::string> CodeGen::GenBinaryInst(sysy::BinaryInst *inst) {
  std::string instruction;

}

std::pair<int, std::string> CodeGen::GenAllocaInst(sysy::AllocaInst *inst) {
  std::string instruction;
  int destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::sp);
  for (auto &var: regManager.varIRegMap){
    if (var.second.first == RegisterManager::VarPos::OnStack) {
      var.second.second += inst->getType()->getSize();
    }
  }
  regManager.varIRegMap.insert({inst->getName(), {RegisterManager::VarPos::OnStack, 0}});
  instruction = "addi sp, sp, " + std::to_string(inst->getType()->getSize()) + endl;
  return {destRegID, instruction};
}

std::string CodeGen::GenStoreInst(sysy::StoreInst *inst) {
  std::string instruction;
  auto src = inst->getValue();
  auto constSrc = dynamic_cast<sysy::ConstantValue *>(src);
  auto destName = inst->getPointer()->getName();
  auto destPos = regManager.varIRegMap.find(destName);
  assert(destPos != regManager.varIRegMap.end());
  if (constSrc) {
    if (constSrc->isInt()) {
      int tmpRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp);
      instruction += "addi " + regManager.intRegs[tmpRegID].second + ", " + "zero, " + std::to_string(constSrc->getInt()) + endl;
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        instruction += "sw " + regManager.intRegs[tmpRegID].second + ", " + std::to_string(destPos->second.second) + "(" + "sp" + ")" + endl;
      }
      else {
        std::cerr << "do not suppport float imm-mem store at this moment" << std::endl;
        exit(1);
      }
      assert(regManager.releaseReg(RegisterManager::RegType::IntReg, tmpRegID)); // in case of release failed
    }
  }
  else {
    // var in registers
    if (src->getType()->isInt()) {
      auto srcReg = regManager.varIRegMap.find(src->getName());
      if (srcReg != regManager.varIRegMap.end()) {
        if (srcReg->second.first == RegisterManager::VarPos::InReg) {
          if (destPos->second.first == RegisterManager::VarPos::OnStack){
            instruction += "sw " + regManager.intRegs[srcReg->second.second].second + ", " + std::to_string(destPos->second.second) + "(sp)" + endl;
          }
          else {
            std::cerr << "do not suppport non on-stack reg-mem store at this moment" << std::endl;
            exit(1);
          }
        }
        else {
          std::cerr << "Error: store to stack" << std::endl;
          exit(1);
        }
      }
      else {
        std::cerr << "Error: cannot find src reg" << std::endl;
        exit(1);
      }
    }
    else {
      // float reg
      std::cerr << "do not support float reg-mem store at this moment" << std::endl;
      exit(1);
    }
  }
  return instruction;
}

std::string CodeGen::instruction_gen(sysy::Instruction *inst) {
  std::string instruction;
  std::string instName = inst->getName();
  auto instType = inst->getKind();
  std::pair<int, std::string> instInfo;
  switch (instType) {
    case sysy::Instruction::kAdd:
    case sysy::Instruction::kSub:
    case sysy::Instruction::kMul:
    case sysy::Instruction::kDiv: {
      sysy::BinaryInst *bInst = dynamic_cast<sysy::BinaryInst *>(inst);
      instInfo = GenBinaryInst(bInst);
    }
  }
}

int RegisterManager::requestReg(RegType rtype, RegHint hint) {
  switch (hint) {
    case zero:
      return 0;
    case ra:
      return 1;
    case sp:
      return 2;
    case gp:
      return 3;
    case tp:
      return 4;
    case t0:
    
  }
}
}