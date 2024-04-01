#include "codegen.h"

namespace codegen {

RegisterManager regManager;

std::string CodeGen::code_gen() {
  std::string assemblyCode;
  assemblyCode += this->module_gen(module);
  return assemblyCode;
}

std::string CodeGen::globalData_gen(){
  std::string data;
  return data;
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
  descriptionCode += space + ".palign 4" + endl;
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
    bbCode += basicBlock_gen(bb);
  }
  std::string assemblyCode;
  std::string headerCode = functionHeader_gen(func);
  std::string calleeRegSaveCode = CalleeRegSave_gen(func);
  std::string calleeRegRestoreCode = CalleeRegRestore_gen(func);
  std::string handleSpCode;

  // handle sp
  int spOff = regManager.spOffset[func];
  assert(spOff < 8192);
  handleSpCode = space + "addi sp, sp, -" + std::to_string(spOff) + endl;

  // TODO literalPoolsCode
  auto funcName = func->getName();
  std::string funcLabel = funcName + ": " + endl;
  assemblyCode = headerCode + funcLabel + calleeRegSaveCode + handleSpCode + bbCode + calleeRegRestoreCode;

  return assemblyCode;
}

std::string CodeGen::basicBlock_gen(sysy::BasicBlock *bb) {
  this->curBBlock = bb;
  std::string bbLabel = this->getBasicBlocksLabel(bb);
  std::string assemblyCode;
  assemblyCode += bbLabel + ":" + endl;
  for (auto &inst: bb->getInstructions()) {
    // auto instType = inst->getKind();
    assemblyCode += instruction_gen(inst.get());
  }
  return assemblyCode;
}

std::string CodeGen::GenBinaryInst(sysy::BinaryInst *inst) {
  std::string instruction;
  auto lhs = inst->getLhs();
  auto rhs = inst->getRhs();
  auto op = inst->getKind();
  std::string optype;
  switch (op) {
  case sysy::Value::Kind::kAdd:
    optype = "add";
    break;
  case sysy::Value::Kind::kSub:
    optype = "sub";
    break;
  case sysy::Value::Kind::kMul:
    optype = "mul";
    break;
  case sysy::Value::Kind::kDiv:
    optype = "div";
    break;
  default:
    std::cerr << "Error: unsupported binary op" << std::endl;
    exit(1);
  }
  if (lhs->getType()->isInt()) {
    auto destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::saved);
    instruction = space + optype + " " + regManager.intRegs[destRegID].second + ", " + regManager.intRegs[regManager.varIRegMap.find(lhs->getName())->second.second].second + ", " + regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second + endl;
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InReg, destRegID};
  }
  else {
    std::cerr << "Error: unsupported binary op" << std::endl;
    exit(1);
  }
  return instruction;
}

std::string CodeGen::GenAllocaInst(sysy::AllocaInst *inst) {
  std::string instruction;
  // int destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::sp);
  // int spOff = regManager.varIRegMap.find(inst->getName())->second.second;
  // instruction = space + "addi sp, sp, -" + std::to_string(spOff) + endl;
  return instruction;
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
      instruction += space + "addi " + regManager.intRegs[tmpRegID].second + ", " + "zero, " + std::to_string(constSrc->getInt()) + endl;
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        instruction += space + "sw " + regManager.intRegs[tmpRegID].second + ", " + std::to_string(destPos->second.second) + "(" + "sp" + ")" + endl;
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
            instruction += space + "sw " + regManager.intRegs[srcReg->second.second].second + ", " + std::to_string(destPos->second.second) + "(sp)" + endl;
            regManager.releaseReg(RegisterManager::RegType::IntReg, srcReg->second.second);
            regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, destPos->second.second};
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
        std::cerr << "src name: " << src->getName() << std::endl;
        for (auto it: regManager.varIRegMap) {
          std::cerr << it.first << " " << it.second.first << " " << it.second.second << std::endl;
        }
        std::cerr << "Inst: " << std::endl;
        inst->print(std::cerr);
        std::cerr << std::endl;
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

std::string CodeGen::GenLoadInst(sysy::LoadInst *inst) {
  std::string instruction;
  auto src = inst->getPointer();
  auto rtype = inst->getType();
  auto srcOffset = 0;
  if (rtype->isInt()) {
    auto destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp);
    auto srcPos = regManager.varIRegMap.find(src->getName());
    if (srcPos != regManager.varIRegMap.end()) {
      if (srcPos->second.first == RegisterManager::VarPos::OnStack) {
        instruction = space + "lw " + regManager.intRegs[destRegID].second + ", " + std::to_string(srcPos->second.second) + "(" + "sp" + ")" + endl;
      }
      else {
        std::cerr << "do not support global vars yet" << std::endl;
        exit(1);
      }
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InReg, destRegID};
    }
    else {
      std::cerr << "Inst: ";
      inst->print(std::cerr);
      std::cerr << std::endl;
      std::cerr << "Error: cannot find src reg" << std::endl;
      exit(1);
    }
  }
  else if (rtype->isFloat()){
    // float reg
    auto destRegID = regManager.requestReg(RegisterManager::RegType::FloatReg, RegisterManager::RegHint::temp);
    auto srcPos = regManager.varFRegMap.find(src->getName());
    if (srcPos != regManager.varFRegMap.end()) {
      if (srcPos->second.first == RegisterManager::VarPos::OnStack) {
        instruction = space + "flw " + regManager.floatRegs[destRegID].second + ", " + std::to_string(srcPos->second.second) + "(" + "sp" + ")" + endl;
      }
      else {
        std::cerr << "do not support global vars yet" << std::endl;
        exit(1);
      }
    }
    else {
      std::cerr << "Error: cannot find src reg" << std::endl;
      exit(1);
    }
  }
  else {
    std::cerr << rtype->isInt() << rtype->isFloat() << std::endl;
    std::cerr << "Error: unsupported type" << std::endl;
    exit(1);
  }
  return instruction;
}


std::string CodeGen::instruction_gen(sysy::Instruction *inst) {
  std::string instruction;
  std::string instName = inst->getName();
  auto instType = inst->getKind();
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv: 
      instruction = GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      instruction = GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst));
      break;
    case sysy::Value::Kind::kStore:
      instruction = GenStoreInst(dynamic_cast<sysy::StoreInst *>(inst));
      break;  
    case sysy::Value::Kind::kLoad:
      instruction = GenLoadInst(dynamic_cast<sysy::LoadInst *>(inst));
      break;
    default:
      // std::cerr << "Error: unsupported instruction" << std::endl;
      // exit(1);
      break;
  }
  return instruction;
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
    case temp:
      for (auto i: this->ItempRegList) {
        if (this->intRegTaken[i] == false) {
          this->intRegTaken[i] = true;
          return i;
        }
      }
      // std::cerr << "Error: no free temp reg" << std::endl;
      // exit(1);
      return 0;
    case saved:
      for (auto i: this->IsavedRegList) {
        if (this->intRegTaken[i] == false) {
          this->intRegTaken[i] = true;
          return i;
        }
      }
      // std::cerr << "Error: no free saved reg" << std::endl;
      // exit(1);
      return 0;
    default:
      std::cerr << "Error: invalid reg hint" << std::endl;
      exit(1);
  }
}


}