#include "codegen.h"

namespace codegen {

RegisterManager regManager;

void CodeGen::code_gen() {
  this->module_gen(module);
  return;
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
        data += name + ":" + endl;
        int totSize = 1;
        for (int k = 0; k < it.second->getNumDims(); k++) {
          totSize *= dynamic_cast<sysy::ConstantValue*>(it.second->getDim(k))->getInt();
        }
        if (it.second->getNumInitVals() > 0) {
          for (int k = 0; k < it.second->getNumInitVals(); k++) {
            data += space + ".word  " + std::to_string(dynamic_cast<sysy::ConstantValue*>(it.second->getInitVals(k))->getInt()) + endl;
          }
          totSize -= it.second->getNumInitVals();
          if (totSize > 0) {
            data += space + ".zero  " + std::to_string(totSize*4) + endl;
          }
        }
        else {
            data += space + ".zero " + std::to_string(totSize*4) + endl; 
        }
      } // array
      else {
        regManager.varIRegMap.insert({name, {RegisterManager::VarPos::Globals, 0}});
        data += name + ":" + endl;
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

void CodeGen::module_gen(sysy::Module *module) {
  std::string compilerIdentifier = "SysY compiler";
  clearModuleLabels(module);
  module->descriptionText += space + ".file" + this->fname + endl;
  module->descriptionText += space + ".attribute risc-v rv64gc little-endian" + endl;
  module->descriptionText += space + ".palign 4" + endl;
  module->descriptionText += space + ".text" + endl;
  // descriptionCode += space + ".globl main" + endl;
  std::map<std::string, sysy::Function*> *funcs = module->getFunctions();
  module->globalDataText += globalData_gen(module); 
  for (auto it = funcs->begin(); it != funcs->end(); it++) {
    std::string fname = it->first;
    sysy::Function *func = it->second;
    auto bbs = func->getBasicBlocks();
    if (bbs.empty()) {
      continue;
    }
    function_gen(func);
  }
  return;
}

void CodeGen::functionHeader_gen(sysy::Function *func){
  // std::string assemblyCode;
  // assemblyCode += space + ".global " + func->getName() + endl;
  // assemblyCode += space + ".type " + func->getName() + ", @function" + endl;
  return;
}

void CodeGen::CalleeRegSave_gen(sysy::Function* func){
  // std::string assemblyCode;
  return;
}

void CodeGen::CalleeRegRestore_gen(sysy::Function* func){
  // std::string assemblyCode;
  return;
}

void CodeGen::function_gen(sysy::Function *func) {
  this->curFunc = func;
  clearFuncInfo(func);
  auto paramTypes = func->getParamTypes();
  auto bbs = func->getBasicBlocks();
  for (auto it = bbs.begin(); it != bbs.end(); it++){
    auto bb = it->get();
    basicBlock_gen(bb);
    for (auto &inst: bb->CoInst) {
      inst.valid = true;
    }
  }
  functionHeader_gen(func);
  CalleeRegSave_gen(func);
  CalleeRegRestore_gen(func);

  // handle sp
  int spOff = regManager.spOffset[func];
  assert(spOff < 8192);
  if (spOff > 0) {
    auto t = sysy::RVInst("addi", "sp", "sp", "-"+std::to_string(spOff));
    t.valid = true;
    func->MetaInst.push_back(t);
    // handleSpCode = space + "addi sp, sp, -" + std::to_string(spOff) + endl;
  }

  // TODO literalPoolsCode
  auto funcName = func->getName();
  // std::string funcLabel = funcName + ": " + endl;
  // assemblyCode = headerCode + funcLabel + calleeRegSaveCode + handleSpCode + bbCode + calleeRegRestoreCode;

  // return assemblyCode;
}

void CodeGen::basicBlock_gen(sysy::BasicBlock *bb) {
  if (bb->getArguments().size() > 8) { assert(0); } // do not support over 8 args
  else {
    for (auto &it: bb->getArguments()) { // formula args
      auto requestReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::arg, -1);
      regManager.varIRegMap[it.get()->getName()] = {RegisterManager::VarPos::InIReg, requestReg};
    }
  }
  this->curBBlock = bb;
  bb->bbLabel = this->getBasicBlocksLabel(bb);
  // assemblyCode += bbLabel + ":" + endl;
  for (auto &inst: bb->getInstructions()) {
    // if (!inst) { continue; }
    // auto instType = inst->getKind();
    instruction_gen(inst.get());
    // std::cout << assemblyCode << std::endl;
    regManager.gc(inst.get()->inst_index);
    // std::cout << "name: " << inst.get()->getName() << " index: " << inst.get()->inst_index << " last used: " << inst.get()->last_used << std::endl;
  }
  return;
}

void CodeGen::GenBinaryCmpInst(sysy::BinaryInst *inst) {
  std::string field1;
  std::string field2;
  std::string field3;
  auto lhs = inst->getLhs();
  auto rhs = inst->getRhs();
  auto op = inst->getKind();
  std::string optype;
  std::string optypei;
  auto lhsReg = regManager.varIRegMap.find(lhs->getName());
  auto rhsReg = regManager.varIRegMap.find(rhs->getName());
  auto cmpResult = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, 0);
  regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, cmpResult};
  // if rhs is imm
  if (rhs->isConstant()) {
    switch (op) {
    case sysy::Value::Kind::kICmpLT:
      optypei = "slti";
      break;
    default:
      break;
    }
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[lhsReg->second.second].second;
    field3 = std::to_string(dynamic_cast<sysy::ConstantValue *>(rhs)->getInt());
    this->curBBlock->CoInst.push_back(sysy::RVInst(optypei, field1, field2, field3));
    return;
  }
  if (lhsReg == regManager.varIRegMap.end() || rhsReg == regManager.varIRegMap.end()) {
    std::cout << lhs->getName() << " " << rhs->getName() << std::endl;
    std::cerr << "Error: unsupported binary op" << std::endl;
    assert(0);
  }
  switch (op) {
  case sysy::Value::Kind::kICmpEQ:
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[lhsReg->second.second].second;
    field3 = regManager.intRegs[rhsReg->second.second].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst("sub", field1, field2, field3));
    optype = "seqz";
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[cmpResult].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2));
    break;
  case sysy::Value::Kind::kICmpGT:
    optype = "sgtz";
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[lhsReg->second.second].second;
    field3 = regManager.intRegs[rhsReg->second.second].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst("sub", field1, field2, field3));
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[cmpResult].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2));
    break;
  case sysy::Value::kICmpLT:
    optype = "sltz";
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[lhsReg->second.second].second;
    field3 = regManager.intRegs[rhsReg->second.second].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst("sub", field1, field2, field3));
    field1 = regManager.intRegs[cmpResult].second;
    field2 = regManager.intRegs[cmpResult].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2));
    break;
  default:
    break;
  }
  // this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
  return;
}

void CodeGen::GenBinaryInst(sysy::BinaryInst *inst) {
  // std::cout << inst->getName() << std::endl;
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  auto lhs = inst->getLhs();
  auto rhs = inst->getRhs();
  auto op = inst->getKind();
  std::string optype;
  std::string optypei;
  switch (op) {
  case sysy::Value::Kind::kAdd:
    optype = "add";
    optypei = "addi";
    break;
  case sysy::Value::Kind::kSub:
    optype = "sub";
    optype = "addi";
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
  // if both sources are imm, go here
  if (lhs->isConstant() && rhs->isConstant()) {
    if (lhs->isInt() && rhs->isInt()) {
      auto src1ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
      field1 = regManager.intRegs[src1ID].second;
      field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      // instruction += space + "li " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
      auto src2ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, inst->last_used);
      field1 = regManager.intRegs[src2ID].second;
      field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      // instruction += space + "li " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
      field1 = regManager.intRegs[src2ID].second;
      field2 = regManager.intRegs[src2ID].second;
      field3 = regManager.intRegs[src1ID].second;
      // instruction += space + optype + " " + field1 + ", " + field2 + ", " + field3 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
      regManager.releaseReg(RegisterManager::RegType::IntReg, src1ID);
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, src2ID};
      regManager.intRegTaken[src2ID].second = inst->last_used;
      // std::cout << inst->getName() << " " << regManager.varIRegMap.find(inst->getName())->second.second << std::endl;
      // assert(0);
      return;
    }
    else {
      assert(false);
    }
  }
  // both are var
  if (!lhs->isConstant() && !rhs->isConstant()) {
    int src1ID, src2ID; // src2 serves as the dest reg
    // src1 on stack, firstly load into regs
    if (regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::OnStack) {
      src1ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
      field1 = regManager.intRegs[src1ID].second;
      field2 = std::to_string(regManager.varIRegMap.find(lhs->getName())->second.second) + "(sp)";
      // instruction += space + "lw " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
    }
    else if (regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg) {
      src1ID = regManager.varIRegMap.find(lhs->getName())->second.second;
    }
    // src2 on stack, firstly load into regs
    if (regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::OnStack) {
      src2ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
      field1 = regManager.intRegs[src2ID].second;
      field2 = std::to_string(regManager.varIRegMap.find(rhs->getName())->second.second) + "(sp)";
      // instruction += space + "lw " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
    }
    else if (regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg) {
      src2ID = regManager.varIRegMap.find(rhs->getName())->second.second;
    }
    field1 = regManager.intRegs[src2ID].second;
    field2 = regManager.intRegs[src1ID].second;
    field3 = regManager.intRegs[src2ID].second;
    // instruction += space + optype + " " + field1 + ", " + field2 + ", " + field3 + endl;
    this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, src2ID};
    if (regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::OnStack) {
      regManager.releaseReg(RegisterManager::IntReg, src1ID);
    }
    regManager.intRegTaken[src2ID].second = inst->last_used;
    // std::cout << instruction << std::endl;
    // std::cout << inst->getName() << std::endl;
    // std::cout << (regManager.varIRegMap.find(inst->getName()) != regManager.varIRegMap.end()) << std::endl;
    return;
  }
  if (!lhs->isConstant() && rhs->isConstant()) {
    auto tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }
  // left is constant, right is var
  if (lhs->isConstant() && !rhs->isConstant()) {
    if (lhs->isInt() && rhs->isInt()) {
      // src2 on stack, firstly load into regs
      if (regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::OnStack) {
        auto src1ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
        field1 = regManager.intRegs[src1ID].second;
        field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
        // instruction += space + "li " + field1 + ", " + field2 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
        assert(regManager.varIRegMap.find(rhs->getName()) != regManager.varIRegMap.end());
        auto destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::saved, inst->last_used);
        field1 = regManager.intRegs[destRegID].second;
        field2 = std::to_string(regManager.varIRegMap.find(rhs->getName())->second.second) + "(sp)";
        // instruction += space + "lw " + field1 + ", " + field2 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
        field1 = regManager.intRegs[destRegID].second;
        field2 = regManager.intRegs[src1ID].second;
        field3 = regManager.intRegs[destRegID].second;
        // instruction += space + optype + " " + field1 + ", " + field2 + ", " + field3 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
        regManager.releaseReg(RegisterManager::IntReg, src1ID);
        regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
        return;
      }
      // addi
      else if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() < 2048 && op == sysy::Value::Kind::kAdd){
        // if imm < 2048, simply addi
        field1 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
        field2 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
        field3 = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
        // instruction += space + optypei + " " + field1 + ", " + field2 + ", " + field3 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst(optypei, field1, field2, field3));
        regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, regManager.varIRegMap.find(rhs->getName())->second.second};
        regManager.intRegTaken[regManager.varIRegMap.find(rhs->getName())->second.second].second = inst->last_used;
        return;
      }
      // src2 in reg, use as the dest, but loading src1 into temp reg is needed to be done first
      else if (regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg) {
        auto src1ID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
        field1 = regManager.intRegs[src1ID].second;
        field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
        // instruction += space + "li " + field1 + ", " + field2 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
        // std::cout << rhs->getName() << " " << regManager.varIRegMap.find(rhs->getName())->second.second << std::endl;
        // assert(0);
        field1 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
        field2 = regManager.intRegs[src1ID].second;
        field3 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
        // instruction += space + optype + " " + field1 + ", " + field2 + ", " + field3 + endl;
        this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
        regManager.releaseReg(RegisterManager::IntReg, src1ID);
        regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, regManager.varIRegMap.find(rhs->getName())->second.second};
        regManager.intRegTaken[regManager.varIRegMap.find(rhs->getName())->second.second].second = inst->last_used;
        return;
      }
      else {
        assert(false);
      }
    }
    else {
      assert(false);
    }
  }
  // else if (lhs->getType()->isInt()) {
  //   auto destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::saved, inst->last_used);
  //   instruction = space + optype + " " + regManager.intRegs[destRegID].second + ", " + regManager.intRegs[regManager.varIRegMap.find(lhs->getName())->second.second].second + ", " + regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second + endl;
  //   regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
  // }
  else {
    std::cerr << "Error: unsupported binary op" << std::endl;
    exit(1);
  }
  return;
}

void CodeGen::GenAllocaInst(sysy::AllocaInst *inst) {
  // std::string instruction;
  // int destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::sp);
  // int spOff = regManager.varIRegMap.find(inst->getName())->second.second;
  // instruction = space + "addi sp, sp, -" + std::to_string(spOff) + endl;
  return;
}

void CodeGen::GenStoreInst(sysy::StoreInst *inst) {
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  // std::string instruction;
  auto src = inst->getValue();
  auto destName = inst->getPointer()->getName();
  // std::cout << destName << ", " << inst->inarray << std::endl;
  auto destPos = regManager.varIRegMap.find(destName);
  // case 1: store 1, %a(dst, on stack or in global); %a is a scalar var
  if (src->isConstant() && (!inst->inarray)) {
    auto constSrc = dynamic_cast<sysy::ConstantValue*>(src);
    if (constSrc->isFloat()) {
      assert(0);
    }
    else {
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        auto dstReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
        field1 = regManager.intRegs[dstReg].second;
        field2 = std::to_string(constSrc->getInt());
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
        field1 = regManager.intRegs[dstReg].second;
        field2 = std::to_string(-1*destPos->second.second) + "(sp)";
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        regManager.releaseReg(RegisterManager::RegType::IntReg, dstReg);
      }
      // else if (destPos->second.first == RegisterManager::VarPos::InIReg) {
      //   field1 = regManager.intRegs[destPos->second.second].second;
      //   field2 = std::to_string(constSrc->getInt());
      //   this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
      // }
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
        field1 = regManager.intRegs[addrReg].second;
        field2 = "%hi(" + destName + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lui", field1, field2));
        auto tmpReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
        field1 = regManager.intRegs[tmpReg].second;
        field2 = std::to_string(constSrc->getInt());
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
        field1 = regManager.intRegs[tmpReg].second;
        field2 = "%lo(" + destName + ")(" + regManager.intRegs[addrReg].second + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
        regManager.releaseReg(RegisterManager::RegType::IntReg, tmpReg);
      }
      return;
    }
  }
  // case 2: store %b(src, in reg), %a(dst, on stack or in global); %a is scalar
  else if (!inst->inarray) {
    auto srcPos = regManager.varIRegMap.find(src->getName())->second;
    assert(srcPos.first == RegisterManager::VarPos::InIReg);
    if (destPos->second.first == RegisterManager::VarPos::OnStack) {
      field1 = regManager.intRegs[srcPos.second].second;
      field2 = std::to_string(-1*destPos->second.second) + "(sp)";
      std::cout << destName << destPos->second.second << std::endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
    }
    else if (destPos->second.first == RegisterManager::VarPos::Globals) {
      auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
      field1 = regManager.intRegs[addrReg].second;
      field2 = "%hi(" + destName + ")";
      this->curBBlock->CoInst.push_back(sysy::RVInst("lui", field1, field2));
      field1 = regManager.intRegs[srcPos.second].second;
      field2 = field2 = "%lo(" + destName + ")(" + regManager.intRegs[addrReg].second + ")";
      this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
    }
    return;
  }
  // case 3: store %b(src, in reg), %a+offset(dst, on stack or in global); %a is an array
  else if (inst->inarray) {
    auto srcPos = regManager.varIRegMap.find(src->getName())->second;
    assert(srcPos.first == RegisterManager::VarPos::InIReg);
    auto offsetValue = inst->getIndex(0);
    if (offsetValue->isConstant()) {
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        field1 = regManager.intRegs[srcPos.second].second;
        field2 = std::to_string(-1*destPos->second.second + 4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(sp)";
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      }
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
        field1 = regManager.intRegs[addrReg].second;
        field2 = "%hi(" + destName + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lui", field1, field2));
        field1 = regManager.intRegs[addrReg].second;
        field2 = regManager.intRegs[addrReg].second;
        field3 = "%lo(" + destName + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, field3));
        field1 = regManager.intRegs[srcPos.second].second;
        field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(" + regManager.intRegs[addrReg].second + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
      }
      return;
    }
    else {
      auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
      assert(regManager.varIRegMap.find(offsetValue->getName())->second.first == RegisterManager::VarPos::InIReg);
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        field1 = regManager.intRegs[addrReg].second;
        field2 = std::to_string(-1*destPos->second.second);
        field3 = "(sp)";
        this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
        field1 = regManager.intRegs[addrReg].second;
        field2 = regManager.intRegs[addrReg].second;
        field3 = regManager.intRegs[regManager.varIRegMap.find(offsetValue->getName())->second.second].second;
        this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
        field1 = regManager.intRegs[srcPos.second].second;
        field2 = regManager.intRegs[addrReg].second;
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      }
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        field1 = regManager.intRegs[addrReg].second;
        field2 = "%hi(" + destName + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lui", field1, field2));
        field1 = regManager.intRegs[addrReg].second;
        field2 = regManager.intRegs[addrReg].second;
        field3 = "%lo(" + destName + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, field3));
        field1 = regManager.intRegs[addrReg].second;
        field2 = regManager.intRegs[addrReg].second;
        field3 = regManager.intRegs[regManager.varIRegMap.find(offsetValue->getName())->second.second].second;
        this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
        field1 = regManager.intRegs[srcPos.second].second;
        field2 = regManager.intRegs[addrReg].second;;
        this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      }
      regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
      return;
    }
  }
  // if (!inst->inarray) {
  //   if (src->isConstant()) {
  //     // constant, but in Value form
  //     auto constSrc = dynamic_cast<sysy::ConstantValue *>(src);
  //     // handle IR like store 1, %a
  //     if (constSrc->isInt()) {
  //       // std::cout << destName << " " << (destPos == regManager.varIRegMap.end()) << std::endl;
  //       // assert(0);
  //       int tmpRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp);
  //       field1 = regManager.intRegs[tmpRegID].second;
  //       field2 = std::to_string(constSrc->getInt());
  //       // instruction += space + "li " + field1 + ", " + field2 + endl;
  //       this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
  //       if (destPos->second.first == RegisterManager::VarPos::OnStack) {
  //         field1 = regManager.intRegs[tmpRegID].second;
  //         field2 = std::to_string(destPos->second.second) + "(" + "sp" + ")";
  //         // instruction += space + "sw " + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //         regManager.releaseReg(RegisterManager::RegType::IntReg, tmpRegID);
  //         return;
  //       }
  //       else if (destPos->second.first == RegisterManager::VarPos::Globals) {
  //         assert(0);
  //         // firstly, compute the offset
  //         // secondly, generate code
  //         // std::cout << "do not support this moment" << std::endl;
  //         // TODO bug exists, should not use newBBName
  //         auto newBBName = this->GAccessBB_gen();
  //         // instruction += newBBName + endl;
  //         int tempRegID1 = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
  //         int tempRegID2 = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, -1);
  //         field1 = regManager.intRegs[tempRegID1].second;
  //         field2 = "%pcrel_hi(" + destName + ")";
  //         // instruction += space + "auipc " + field1 + ", %pcrel_hi" + "(" + destName + ")" + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("auipc", field1, field2));
  //         field1 = regManager.intRegs[tempRegID2].second;
  //         field2 = regManager.intRegs[tempRegID1].second;
  //         field3 = "%pcrel_lo(" + newBBName + ")";
  //         // instruction += space + "addi " + field1 + ", " + field2 + ", %pcrel_lo(" + newBBName + ")" + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, field3));
  //         field1 = regManager.intRegs[tempRegID1].second;
  //         field2 = std::to_string(constSrc->getInt());
  //         // instruction += space + "li " + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
  //         // ??????
  //         // instruction += space + regManager.intRegs[tempRegID1].second + "(" + regManager.intRegs[tempRegID2].second + ")" + endl;
  //         regManager.releaseReg(RegisterManager::RegType::IntReg, tmpRegID);
  //         return;
  //       }
  //       else {
  //         std::cerr << "do not suppport float imm-mem store at this moment" << std::endl;
  //         exit(1);
  //       }
  //       // assert(); // in case of release failed
  //     }
  //   }
  //   else {
  //     // src is var in reg
  //     auto srcReg = regManager.varIRegMap.find(src->getName());
  //     assert(srcReg != regManager.varIRegMap.end());
  //     // if (srcReg == regManager.varIRegMap.end()) {
  //     //   std::cout << src->getName() << " " << src->getKind() << std::endl;
  //     //   assert(0);
  //     // }
  //     if (srcReg->second.first == RegisterManager::VarPos::InIReg) {
  //       if (destPos->second.first == RegisterManager::VarPos::OnStack) {
  //         field1 = regManager.intRegs[srcReg->second.second].second;
  //         field2 = std::to_string(destPos->second.second) + "(sp)";
  //         // instruction += space + "sw " + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //         regManager.releaseReg(RegisterManager::RegType::IntReg, srcReg->second.second);
  //         regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, destPos->second.second};
  //         return;
  //       }
  //       else { assert(0); }
  //     }
  //     else if (srcReg->second.first == RegisterManager::VarPos::Imm) {
  //       if (destPos->second.first == RegisterManager::VarPos::OnStack) {
  //         int tmpRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, 0);
  //         field1 = regManager.intRegs[tmpRegID].second;
  //         field2 = std::to_string(srcReg->second.second);
  //         // instruction += space + "li " + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
  //         field1 = regManager.intRegs[tmpRegID].second;
  //         field2 = std::to_string(destPos->second.second) + "(sp)";
  //         // instruction += space + "sw " + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //       }
  //     }
  //     else { assert(0); }
  //   }
  // }
  // else {
  //   // var is array
  //   if (src->isConstant()) {
  //     // src is const
  //     auto constSrc = dynamic_cast<sysy::ConstantValue *>(src);
  //     if (constSrc->isInt()) {
  //       int tmpRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp);
  //       field1 = regManager.intRegs[tmpRegID].second;
  //       field2 = std::to_string(constSrc->getInt());
  //       // instruction += space + "li " + field1 + ", " + field2 + endl;
  //       this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
  //       if (destPos->second.first == RegisterManager::VarPos::OnStack) {
  //         if (inst->getOffset()->isConstant()) {
  //           // offset is const
  //           field1 = regManager.intRegs[tmpRegID].second;
  //           field2 = std::to_string(destPos->second.second + dynamic_cast<sysy::ConstantValue*>(inst->getOffset())->getInt()*4) + "(sp)";
  //           // instruction += space + "sw " + field1 + ", " + field2 + endl;
  //           this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //           regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, destPos->second.second + dynamic_cast<sysy::ConstantValue*>(inst->getOffset())->getInt()};
  //           regManager.releaseReg(RegisterManager::RegType::IntReg, tmpRegID);
  //           return;
  //         }
  //         else {
  //           // offset is var
  //           auto offname = inst->getOffset()->getName();
  //           assert(regManager.varIRegMap.find(offname) != regManager.varIRegMap.end());
  //           if (regManager.varIRegMap.find(offname)->second.first == RegisterManager::VarPos::InIReg) {
  //             // offset is in reg
  //             auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare);
  //             field1 = regManager.intRegs[addrReg].second;
  //             field2 = "sp";
  //             field3 = std::to_string(regManager.varIRegMap.find(destName)->second.second);
  //             // instruction += space + "addi " + field1 + ", sp, " + field3 + endl;
  //             this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, field3));
  //             field1 = regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second;
  //             field2 = regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second;
  //             field3 = regManager.intRegs[addrReg].second;
  //             // instruction += space + "add " + field1 + ", " + field2 + ", " + field3 + endl;
  //             this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
  //             field1 = regManager.intRegs[tmpRegID].second;
  //             field2 = "0(" + regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second + ")";
  //             // instruction += space + "sw " + field1 + ", 0(" + regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second + ")" + endl;
  //             this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //             regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, -1}; // todo: actually this is unnecessary
  //             regManager.releaseReg(RegisterManager::RegType::IntReg, tmpRegID);
  //             regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
  //             return;
  //           }
  //           else {
  //             // offset is on stack
  //             assert(0);
  //           }
  //         }
  //       }
  //     }
  //     else { assert(0); }
  //   }
  //   else {
  //     // src is var
  //     assert(regManager.varIRegMap.find(src->getName()) != regManager.varIRegMap.end());
  //     auto srcReg = regManager.varIRegMap.find(src->getName())->second.second;
  //     assert(regManager.varIRegMap.find(src->getName())->second.first == RegisterManager::VarPos::InIReg); // src should be in reg
  //     if (destPos->second.first == RegisterManager::VarPos::OnStack) {
  //       if (inst->getOffset()->isConstant()) {
  //         // offset is const
  //         field1 = regManager.intRegs[srcReg].second;
  //         field2 = std::to_string(destPos->second.second + dynamic_cast<sysy::ConstantValue*>(inst->getOffset())->getInt()) + "(sp)";
  //         // instruction += space + "sw" + field1 + ", " + field2 + endl;
  //         this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //         regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, destPos->second.second + dynamic_cast<sysy::ConstantValue*>(inst->getOffset())->getInt()};
  //         return;
  //       }
  //       else {
  //         // offset is var
  //         auto offname = inst->getOffset()->getName();
  //         assert(regManager.varIRegMap.find(offname) != regManager.varIRegMap.end());
  //         if (regManager.varIRegMap.find(offname)->second.first == RegisterManager::VarPos::InIReg) {
  //           // offset is in reg
  //           field1 = regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second;
  //           field2 = regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second;
  //           field3 = "sp";
  //           // instruction += space + "add " + field1 + ", " + field2 + ", sp" + endl;
  //           this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
  //           field1 = regManager.intRegs[srcReg].second;
  //           field2 = "0(" + regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second + ")";
  //           // instruction += space + "sw " + field1 + ", 0(" + regManager.intRegs[regManager.varIRegMap.find(offname)->second.second].second + ")" + endl;
  //           this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
  //           regManager.varIRegMap[src->getName()] = {RegisterManager::VarPos::OnStack, -1}; // todo: actually this is unnecessary
  //           return;
  //         }
  //         else {
  //           // offset is on stack
  //           assert(0);
  //         }
  //       }
  //     }
  //   }
  // } 
  // return;
}

void CodeGen::GenLoadInst(sysy::LoadInst *inst) {
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  std::string instruction;
  int destRegID;
  auto src = inst->getPointer();
  auto srcPos = regManager.varIRegMap.find(src->getName());
  // allocate a reg for dest
  if (inst->getType()->isInt()) {
    if (!inst->getNumIndices()) {
      // case 1: load %v(dst, in reg), %a(src, scalar, on stack or in global)
      destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
      if (srcPos->second.first == RegisterManager::VarPos::OnStack) {
        field1 = regManager.intRegs[destRegID].second;
        field2 = std::to_string(srcPos->second.second) + "(sp)";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
      }
      else {
        assert(srcPos->second.first == RegisterManager::VarPos::Globals);
        field1 = regManager.intRegs[destRegID].second;
        field2 = "%hi(" + src->getName() + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lui", field1, field2));
        // field1 = regManager.intRegs[destRegID].second;
        // field2 = regManager.intRegs[destRegID].second;
        // field3 = "%lo(" + src->getName() + ")";
        // this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, field3));
        field1 = regManager.intRegs[destRegID].second;
        field2 = "%lo(" + src->getName() + ")(" + regManager.intRegs[destRegID].second + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2)); 
      }
    }
    // case 2: load %v (dst, in reg), %arr+%t(array, on stack or in global)
    else {
      destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
      auto offset = inst->getIndex(0);
    }
  }
  else {
    destRegID = regManager.requestReg(RegisterManager::RegType::FloatReg, RegisterManager::RegHint::temp, inst->last_used);
    regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InFReg, destRegID};
    field1 = regManager.floatRegs[destRegID].second;
    field2 = std::to_string(srcPos->second.second) + "(" + "sp" + ")";
    // instruction = space + "flw " + field1 + ", " + field2 + endl;
    this->curBBlock->CoInst.push_back(sysy::RVInst("flw", field1, field2));
  }
  return;
}

void CodeGen::GenReturnInst(sysy::ReturnInst* inst) {
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  // std::string instruction;
  if (inst->hasReturnValue()) {
    auto retVname = inst->getReturnValue()->getName();
    auto retV = inst->getReturnValue();
    auto retVtype = inst->getReturnValue()->getType();
    if (retV->isConstant()) { 
      if (retVtype->isInt()) {
        field1 = "a0";
        field2 = std::to_string(dynamic_cast<sysy::ConstantValue*>(retV)->getInt());
        // instruction += space + "li a0, " + field2 + endl; 
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
      }
    }
    else {
      if (retVtype->isInt()) {
        if (regManager.varIRegMap.find(retV->getName())->second.first == RegisterManager::VarPos::OnStack) {
          field1 = "a0";
          field2 = std::to_string(regManager.varIRegMap.find(retV->getName())->second.second) + "(sp)";
          // instruction += space + "lw a0, " + field2 + endl;
          this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
        }
        else {
          // on reg
          field1 = "a0";
          field2 = regManager.intRegs[regManager.varIRegMap.find(retV->getName())->second.second].second;
          // instruction += space + "mv a0, " + field2 + endl;
          this->curBBlock->CoInst.push_back(sysy::RVInst("mv", field1, field2));
        }
      }
    }
  }
  else {
    // bare return, used for void functions
  }
  // instruction += space + "ret" + endl;
  this->curBBlock->CoInst.push_back(sysy::RVInst("ret"));
  return;
}

void CodeGen::GenCondBrInst(sysy::CondBrInst* inst) {
  // std::cout << *inst->getCondition() << std::endl; 
  // assert(0);
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  // std::string instruction;
  auto cond = inst->getCondition();
  auto condName = cond->getName();
  // std::cout << regManager.varIRegMap.find(condName)->second.second << std::endl;
  // assert(regManager.varIRegMap.find(condName) != regManager.varIRegMap.end());
  if (regManager.varIRegMap.find(condName) == regManager.varIRegMap.end()) {
    std::cout << condName << std::endl;
    assert(0);
  }
  auto thenBlockName = this->getBasicBlocksLabel(inst->getThenBlock());
  auto elseBlockName = this->getBasicBlocksLabel(inst->getElseBlock());
  if (cond->isConstant()) {
    std::cerr << "do not support constant cond now" << std::endl;
    assert(0);
  }
  else {
    // int destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
    // field1 = regManager.intRegs[destRegID].second;
    field1 = regManager.intRegs[regManager.varIRegMap.find(condName)->second.second].second;
    field2 = thenBlockName;
    // instruction += space + "beqz " + field1 + ", " + field2 + endl;
    this->curBBlock->CoInst.push_back(sysy::RVInst("beqz", field1, field2));
    field1 = elseBlockName;
    // instruction += space + "j " + field1 + endl;
    this->curBBlock->CoInst.push_back(sysy::RVInst("j", field1));
  }
  // std::cout << instruction << std::endl;
  // assert(0);
  return;
}

void CodeGen::GenUncondBrInst(sysy::UncondBrInst* inst) {
  // std::string instruction;
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  field1 = this->getBasicBlocksLabel(inst->getBlock());
  // instruction += space + "j " + field1 + endl;
  this->curBBlock->CoInst.push_back(sysy::RVInst("j", field1));
  return;
}

void CodeGen::GenUnaryInst(sysy::UnaryInst* inst) {
  // std::string instruction;
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  auto op = inst->getKind();
  if (op == sysy::Value::Kind::kNeg) {
    if (inst->getOperand()->isConstant()) {
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::Imm, dynamic_cast<sysy::ConstantValue *>(inst->getOperand())->getInt()*-1};
      return;
    }
    auto srcName = inst->getOperand()->getName();
    assert(regManager.varIRegMap.find(srcName) != regManager.varIRegMap.end());
    auto src = regManager.varIRegMap.find(srcName);
    if (src->second.first == RegisterManager::VarPos::OnStack) {
      auto tempReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::dontCare, 0);
      field1 = regManager.intRegs[tempReg].second;
      field2 = std::to_string(src->second.second) + "(sp)";
      // instruction += space + "lw " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
      field1 = regManager.intRegs[tempReg].second;
      field2 = regManager.intRegs[tempReg].second;
      // instruction += space + "neg " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("neg", field1, field2));
      field1 = regManager.intRegs[tempReg].second;
      field2 = std::to_string(src->second.second) + "(sp)";
      // instruction += space + "sw " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      regManager.releaseReg(RegisterManager::RegType::IntReg, tempReg);
      return;
    }
    else if (src->second.first == RegisterManager::VarPos::InIReg){
      field1 = regManager.intRegs[src->second.second].second;
      field2 = regManager.intRegs[src->second.second].second;
      // instruction += space + "neg " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("neg", field1, field2));
      return;
    }
    else {
      // on global
      std::cerr << " do not support global" << std::endl;
      assert(0);
    }
  }
  else {
    assert(0);
  }
}

void CodeGen::GenCallInst(sysy::CallInst* inst, int dstRegID) {
  // std::string instruction;
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  if (inst->getArguments().size() > 8) { assert(0); } // do not support over 8 args
  else {
    for (auto &it: inst->getArguments()) {
      // std::cout << it.getValue()->getName() << std::endl;
      auto requestReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::arg, -1);
      auto srcArgReg = regManager.varIRegMap.find(it.getValue()->getName())->second.second;
      field1 = regManager.intRegs[requestReg].second;
      field2 = regManager.intRegs[srcArgReg].second;
      // instruction += space + "mv " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("mv", field1, field2));
      regManager.varIRegMap[it.getValue()->getName()] = {RegisterManager::VarPos::InIReg, requestReg};
    }
  }
  // actually, just a simple call
  field1 = inst->getCallee()->getName();
  // instruction += space + "call " + field1 + endl;
  this->curBBlock->CoInst.push_back(sysy::RVInst("call", field1));
  regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, 10};
  return;
}

void CodeGen::instruction_gen(sysy::Instruction *inst) {
  // std::string instruction;
  std::string instName = inst->getName();
  auto instType = inst->getKind();
  switch (instType) {
    case sysy::Value::Kind::kAdd:
    case sysy::Value::Kind::kSub:
    case sysy::Value::Kind::kMul:
    case sysy::Value::Kind::kDiv: 
      GenBinaryInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kAlloca:
      GenAllocaInst(dynamic_cast<sysy::AllocaInst *>(inst));
      break;
    case sysy::Value::Kind::kStore:
      GenStoreInst(dynamic_cast<sysy::StoreInst *>(inst));
      break;  
    case sysy::Value::Kind::kLoad:
      GenLoadInst(dynamic_cast<sysy::LoadInst *>(inst));
      break;
    case sysy::Value::Kind::kReturn:
      GenReturnInst(dynamic_cast<sysy::ReturnInst *>(inst));
      break;
    case sysy::Value::Kind::kCondBr:
      GenCondBrInst(dynamic_cast<sysy::CondBrInst *>(inst));
      break;
    case sysy::Value::Kind::kBr:
      GenUncondBrInst(dynamic_cast<sysy::UncondBrInst *>(inst));
      break;
    case sysy::Value::Kind::kICmpEQ:
    case sysy::Value::Kind::kICmpGT:
    case sysy::Value::Kind::kICmpLT:
      GenBinaryCmpInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kNeg:
      GenUnaryInst(dynamic_cast<sysy::UnaryInst *>(inst));
      break;
    case sysy::Value::Kind::kCall:
      GenCallInst(dynamic_cast<sysy::CallInst *>(inst), 10);
      break;
    default:
      break;
  }
  return;
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
    case dontCare:
      for (auto i: this->IdontCareRegList) {
        if (this->intRegTaken[i].first == false) {
          this->intRegTaken[i].first = true;
          this->intRegTaken[i].second = last_used;
          return i;
        }
      }
      return 0;
    case arg:
    for (auto i: this->IArgRegList) {
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