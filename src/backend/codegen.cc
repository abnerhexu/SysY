#include "codegen.h"

namespace codegen {

RegisterManager regManager;

std::string CodeGen::code_gen() {
  std::string assemblyCode;
  assemblyCode += this->module_gen(module);
  return assemblyCode;
}

std::string CodeGen::globalData_gen(sysy::Module* module){
  std::string data;
  auto GlobalValues = module->getGlobalValues();
  for (auto &it: *GlobalValues) {
    auto name = it.first;
    if (dynamic_cast<sysy::PointerType*>(it.second->getType())->getBaseType()->isInt()) {
      // int
      if (it.second->getNumDims() > 0) {
        regManager.varIRegMap.insert({name, {RegisterManager::VarPos::Globals, 0}});
        data += ".gblock_" + name + ":" + endl;
        if (it.second->getNumInitVals() > 0) {
          int totSize = 1;
          for (int k = 0; k < it.second->getNumDims(); k++) {
            totSize *= dynamic_cast<sysy::ConstantValue*>(it.second->getDim(k))->getInt();
          }
          for (int k = 0; k < it.second->getNumInitVals(); k++) {
            data += space + ".word  " + std::to_string(dynamic_cast<sysy::ConstantValue*>(it.second->getInitVals(k))->getInt()) + endl;
          }
          totSize -= it.second->getNumInitVals();
          if (totSize > 0) {
            data += space + ".zero  " + std::to_string(totSize*4) + endl;
          }
        }
      } // array
      else {
        regManager.varIRegMap.insert({name, {RegisterManager::VarPos::Globals, 0}});
        data += ".gblock_" + name + ":" + endl;
        if (it.second->getNumInitVals() > 0) {
          data += space + ".word  " + std::to_string(dynamic_cast<sysy::ConstantValue*>(it.second->init())->getInt()) + endl;
        } // with init
        else {
          data += space + ".zero  4" + endl;
        } // without init
      } // scalar
    }
    else {
      std::cout << it.first << " " << it.second->getType()->getPointerType(it.second->getType())->isInt() << std::endl;
    }
  }
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
  // descriptionCode += space + ".globl main" + endl;
  std::map<std::string, sysy::Function*> *funcs = module->getFunctions();
  dataCode += globalData_gen(module); 
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
  // assemblyCode += space + ".global " + func->getName() + endl;
  // assemblyCode += space + ".type " + func->getName() + ", @function" + endl;
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
  if (spOff > 0) {
    handleSpCode = space + "addi sp, sp, -" + std::to_string(spOff) + endl;
  }

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
    regManager.gc(inst.get()->inst_index);
    // std::cout << "name: " << inst.get()->getName() << " index: " << inst.get()->inst_index << " last used: " << inst.get()->last_used << std::endl;
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
    auto destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::saved, inst->last_used);
    instruction = space + optype + " " + regManager.intRegs[destRegID].second + ", " + regManager.intRegs[regManager.varIRegMap.find(lhs->getName())->second.second].second + ", " + regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second + endl;
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
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
  if (constSrc) {
    // handle IR like store 1, %a
    if (constSrc->isInt()) {
      int tmpRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp);
      instruction += space + "li " + regManager.intRegs[tmpRegID].second + ", " + std::to_string(constSrc->getInt()) + endl;
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        instruction += space + "sw " + regManager.intRegs[tmpRegID].second + ", " + std::to_string(destPos->second.second) + "(" + "sp" + ")" + endl;
      }
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        // firstly, compute the offset
        // secondly, generate code
        // std::cout << "do not support this moment" << std::endl;
        auto newBBName = this->GAccessBB_gen();
        instruction += newBBName + endl;
        int tempRegID1 = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
        int tempRegID2 = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
        instruction += space + "auipc " + regManager.intRegs[tempRegID1].second + ", %pcrel_hi" + "(" + destName + ")" + endl;
        instruction += space + "addi " + regManager.intRegs[tempRegID2].second + ", " + regManager.intRegs[tempRegID1].second + ", %pcrel_lo(" + newBBName + ")" + endl;
        instruction += space + "li " + regManager.intRegs[tempRegID1].second + ", " + std::to_string(constSrc->getInt()) + endl;
        instruction += space + regManager.intRegs[tempRegID1].second + "(" + regManager.intRegs[tempRegID2].second + ")" + endl;
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
        if (srcReg->second.first == RegisterManager::VarPos::InIReg) {
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
  int destRegID;
  auto src = inst->getPointer();
  auto srcPos = regManager.varIRegMap.find(src->getName());
  // allocate a reg for dest
  if (inst->getType()->isInt()) {
    destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
    instruction = space + "lw " + regManager.intRegs[destRegID].second + ", " + std::to_string(srcPos->second.second) + "(" + "sp" + ")" + endl;
  }
  else {
    destRegID = regManager.requestReg(RegisterManager::RegType::FloatReg, RegisterManager::RegHint::temp, inst->last_used);
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
    instruction = space + "flw " + regManager.floatRegs[destRegID].second + ", " + std::to_string(srcPos->second.second) + "(" + "sp" + ")" + endl;
  }
  return instruction;
}

std::string GenReturnInst(sysy::ReturnInst* inst) {
  std::string instruction;
  if (inst->hasReturnValue()) {
    auto retVname = inst->getReturnValue()->getName();
    auto retV = inst->getReturnValue();
    auto retVtype = inst->getReturnValue()->getType();
    if (retV->isConstant()) { 
      if (retVtype->isInt()) {
        instruction += space + "li a0, " + std::to_string(dynamic_cast<sysy::ConstantValue*>(retV)->getInt()) + endl; 
      }
    }
    else {
      if (retVtype->isInt()) {
        if (regManager.varIRegMap.find(retV->getName())->second.first == RegisterManager::VarPos::OnStack) {
          instruction += space + "lw a0, " + std::to_string(regManager.varIRegMap.find(retV->getName())->second.second) + "(sp)" + endl;
        }
        else {
          // on reg
          instruction += space + "mv a0, " + regManager.intRegs[regManager.varIRegMap.find(retV->getName())->second.second].second + endl;
        }
      }
    }
  }
  else {
    // bare return, used for void functions
  }
  instruction += space + "ret" + endl;
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
    case sysy::Value::Kind::kReturn:
      instruction = GenReturnInst(dynamic_cast<sysy::ReturnInst *>(inst));
    default:
      break;
  }
  return instruction;
}

int RegisterManager::requestReg(RegType rtype, RegHint hint, int last_used) {
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
        if (this->intRegTaken[i].first == false) {
          this->intRegTaken[i].first = true;
          this->intRegTaken[i].second = last_used;
          return i;
        }
      }
      return 0;
    case saved:
      for (auto i: this->IsavedRegList) {
        if (this->intRegTaken[i].first == false) {
          this->intRegTaken[i].first = true;
          this->intRegTaken[i].second = last_used;
          return i;
        }
      }
      return 0;
    default:
      std::cerr << "Error: invalid reg hint" << std::endl;
      exit(1);
  }
}

void RegisterManager::gc(int inst_index) {
  for (auto &it: this->intRegTaken) {
    if (it.second <= inst_index) {
      it.first = false;
    }
  }
  for (auto &it: this->floatRegTaken) {
    if (it.second <= inst_index) {
      it.first = false;
    }
  }
}



}