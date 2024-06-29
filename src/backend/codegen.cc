#include "codegen.h"
#include "../debug.h"
namespace codegen {

RegisterManager regManager;

void CodeGen::code_gen() {
  this->module_gen(module);
  return;
}

std::string CodeGen::globalData_gen(sysy::Module* module){
  std::string data = "  .text\n";
  std::string des;
  std::string dd;
  std::string dsize;
  std::string alignType;
  auto GlobalValues = module->getGlobalValues();
  for (auto &it: *GlobalValues) {
    auto name = it.first;
    dd = "";
    dsize = "";
    std::string bss = ".bss";
    if (dynamic_cast<sysy::PointerType*>(it.second->getType())->getBaseType()->isInt()) {
      // int
      if (it.second->getNumDims() > 0) {
        alignType = "3";
        regManager.varIRegMap.insert({name, {RegisterManager::VarPos::Globals, 0}});
        dd += name + ":" + endl;
        int totSize = 1;
        for (int k = 0; k < it.second->getNumDims(); k++) {
          totSize *= dynamic_cast<sysy::ConstantValue*>(it.second->getDim(k))->getInt();
        }
        dsize = std::to_string(totSize*4) + "\n";
        module->staticSize = totSize*4;
        if (it.second->getNumInitVals() > 0) {
          bss = ".data";
          for (int k = 0; k < it.second->getNumInitVals(); k++) {
            dd += space + ".word  " + std::to_string(dynamic_cast<sysy::ConstantValue*>(it.second->getInitVals(k))->getInt()) + endl;
          }
          totSize -= it.second->getNumInitVals();
          if (totSize > 0) {
            dd += space + ".zero  " + std::to_string(totSize*4) + endl;
          }
        }
        else {
            dd += space + ".zero " + std::to_string(totSize*4) + endl; 
        }
      } // array
      else {
        alignType = "2";
        regManager.varIRegMap.insert({name, {RegisterManager::VarPos::Globals, 0}});
        dd += name + ":" + endl;
        dsize = "4\n";
        module->staticSize = 4;
        if (it.second->getNumInitVals() > 0) {
          auto initValInt = dynamic_cast<sysy::ConstantValue*>(it.second->init())->getInt();
          if (initValInt == 0) {
            bss = ".bss";
          }
          else {
            bss = ".data";
          }
          dd += space + ".word  " + std::to_string(initValInt) + endl;
        } // with init
        else {
          dd += space + ".zero  4" + endl;
          bss = ".bss";
        } // without init
      } // scalar
    }
    else {
      std::cout << it.first << " " << it.second->getType()->getPointerType(it.second->getType())->isInt() << std::endl;
    }
    des = "  .globl " + name + "\n  " + bss +"\n  .align	" + alignType + "\n  .type " + name + ", @object\n  .size	" + name + ", " + dsize;
    data += des + dd;
  }
  return data;
}

void CodeGen::module_gen(sysy::Module *module) {
  std::string compilerIdentifier = "SysY compiler";
  clearModuleLabels(module);
  module->descriptionText += "  .file \"" + module->srcFile + "\"\n" + "  .option pic\n" + "	.attribute arch, \"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0\"\n";
  module->descriptionText += "	.attribute unaligned_access, 0\n  .attribute stack_align, 16\n";
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
  // auto bbs = func->getBasicBlocks();
  // for (auto it = bbs.begin(); it != bbs.end(); it++){
  //   auto bb = it->get();
  //   basicBlock_gen(bb);
  //   for (auto &inst: bb->CoInst) {
  //     inst.valid = true;
  //   }
  // }
  auto bb = func->getBasicBlocks().begin()->get();
  std::vector<sysy::BasicBlock *> bbs;
  bbs.push_back(bb);
  while(!bbs.empty()){
    auto bbtop = bbs[bbs.size()-1];
    bbs.pop_back();
    basicBlock_gen(bbtop);
    for (auto &inst: bbtop->CoInst){
      inst.valid = true;
    }
    for (int i = bbtop->getNumSuccessors()-1; i >= 0; i--){
      bbs.push_back(bbtop->getSuccessors()[i]);
    }
  }
  functionHeader_gen(func);
  CalleeRegSave_gen(func);
  CalleeRegRestore_gen(func);

  // handle sp
  int spOff = regManager.spOffset[func];
  if (spOff >= 8192) {
    std::cerr << debug::colorize("Warning", debug::Color::yellow) << ": stack size in typical Linux platform should less than 8KB" << std::endl;
    assert(spOff < 8192);
  }
  if (spOff > 0) {
    auto t = sysy::RVInst("addi", "sp", "sp", "-"+std::to_string(spOff));
    t.valid = true;
    func->MetaInst.push_back(t);
    func->PostInst.push_back(sysy::RVInst("addi", "sp", "sp", std::to_string(spOff)));
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
  auto resName = inst->getName();
  auto resReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
  regManager.varIRegMap[resName] = {RegisterManager::VarPos::InIReg, resReg};

  if (op == sysy::Value::Kind::kICmpEQ) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() == dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kICmpNE) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() != dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kICmpGT) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() > dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kICmpGE) {
    auto addiReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
    regManager.releaseReg(RegisterManager::RegType::IntReg, addiReg);
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() >= dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[addiReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[addiReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kICmpLT) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() < dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kICmpLE) {
    auto addiReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
    regManager.releaseReg(RegisterManager::RegType::IntReg, addiReg);
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() <= dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(lhs)->getInt());
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, lhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[addiReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = std::to_string(dynamic_cast<sysy::ConstantValue*>(rhs)->getInt());
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, rhsInt));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", regManager.intRegs[addiReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sub", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("sgtz", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[resReg].second, regManager.intRegs[addiReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kAnd) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() && dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = dynamic_cast<sysy::ConstantValue*>(lhs)->getInt();
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      if (lhsInt == 0) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      }
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = dynamic_cast<sysy::ConstantValue*>(rhs)->getInt();
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      if (rhsInt == 0) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second));
      }
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[lhsReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[rhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("and", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }

  if (op == sysy::Value::Kind::kOr) {
    if (lhs->isConstant() && rhs->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue*>(lhs)->getInt() || dynamic_cast<sysy::ConstantValue*>(rhs)->getInt()) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "0"));
      }
      return;
    }
    else if (lhs->isConstant() && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsInt = dynamic_cast<sysy::ConstantValue*>(lhs)->getInt();
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      if (lhsInt != 0) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[rhsReg].second));
      }
      return;
    }
    else if ((!lhs->isConstant()) && rhs->isConstant()) {
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto rhsInt = dynamic_cast<sysy::ConstantValue*>(rhs)->getInt();
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      if (rhsInt != 0) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[resReg].second, "1"));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second));
      }
      return;
    }
    else if ((!lhs->isConstant()) && (!rhs->isConstant())) {
      assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
      auto lhsReg = regManager.varIRegMap[lhs->getName()].second;
      auto rhsReg = regManager.varIRegMap[rhs->getName()].second;
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[lhsReg].second, regManager.intRegs[lhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("snez", regManager.intRegs[rhsReg].second, regManager.intRegs[rhsReg].second));
      this->curBBlock->CoInst.push_back(sysy::RVInst("or", regManager.intRegs[resReg].second, regManager.intRegs[lhsReg].second, regManager.intRegs[rhsReg].second));
      return;
    }
    else {
      std::cerr << "lhs or rhs error!" << std::endl;
      assert(0);
    }
  }
}

void CodeGen::GenBinaryInst(sysy::BinaryInst *inst) {
  // std::cout << inst->getName() << " " << inst->getLhs()->getName() << " " << inst->getRhs()->getName() << std::endl;
  // std::cout << inst->getName() << std::endl;
  auto curbbName = this->getBasicBlocksLabel(this->curBBlock);
  std::string field1, field2, field3;
  auto lhs = inst->getLhs();
  auto rhs = inst->getRhs();
  auto op = inst->getKind();
  auto dst = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
  regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, dst};
  if (lhs->isConstant() && rhs->isConstant()) {
    auto constInt1 = dynamic_cast<sysy::ConstantValue*>(lhs)->getInt();
    auto constInt2 = dynamic_cast<sysy::ConstantValue*>(rhs)->getInt();
    int res = 0;
    switch (op) {
    case sysy::Value::Kind::kAdd:
      res = constInt1 + constInt2;
      break;
    case sysy::Value::Kind::kSub:
      res = constInt1 - constInt2;
      break;
    case sysy::Value::Kind::kMul:
      res = constInt1 * constInt2;
      break;
    case sysy::Value::Kind::kDiv:
      res = constInt1 / constInt2;
      break;
    case sysy::Value::Kind::kRem:
      res = constInt1 % constInt2;
    default:
      break;
    }
    this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[dst].second, std::to_string(res)));
    return;
  }
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
    case sysy::Value::Kind::kRem:
      optype = "rem";
      break;
    default:
      std::cerr << "unsupported optype!" << std::endl;
      assert(0);
      break;
  }
  if ((!lhs->isConstant()) && (!rhs->isConstant())) {
    assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
    assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
    field1 = regManager.intRegs[dst].second;
    field2 = regManager.intRegs[regManager.varIRegMap.find(lhs->getName())->second.second].second;
    field3 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
    this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field3));
    return;
  }
  if ((!lhs->isConstant()) && (rhs->isConstant())) {
    assert(regManager.varIRegMap.find(lhs->getName())->second.first == RegisterManager::VarPos::InIReg);
    field1 = regManager.intRegs[dst].second;
    field2 = regManager.intRegs[regManager.varIRegMap.find(lhs->getName())->second.second].second;
    int imm = dynamic_cast<sysy::ConstantValue*>(rhs)->getInt();
    if (imm < 2048 && imm > -2048) {
      switch (op) {
      case sysy::Value::kAdd:
        this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, std::to_string(imm)));
        break;
      case sysy::Value::kSub:
        this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, std::to_string(-1*imm)));
        break;
      case sysy::Value::kMul:
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
        this->curBBlock->CoInst.push_back(sysy::RVInst("mul", field1, field1, field2));
        break;
      case sysy::Value::kDiv:
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
        this->curBBlock->CoInst.push_back(sysy::RVInst("div", field1, field2, field1));
        break;
      case sysy::Value::kRem:
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
        this->curBBlock->CoInst.push_back(sysy::RVInst("rem", field1, field2, field1));
        break;
      default:
        break;
      }
      return;
    }
    else {
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
      this->curBBlock->CoInst.push_back(sysy::RVInst(optype, field1, field2, field1));
      return;
    }
    return;
  }
  if ((lhs->isConstant()) && (!rhs->isConstant())) {
    assert(regManager.varIRegMap.find(rhs->getName())->second.first == RegisterManager::VarPos::InIReg);
    field1 = regManager.intRegs[dst].second;
    field2 = regManager.intRegs[regManager.varIRegMap.find(rhs->getName())->second.second].second;
    int imm = dynamic_cast<sysy::ConstantValue*>(lhs)->getInt();
    if (imm < 2048 && imm > -2048 && op == sysy::Value::kAdd) {
      this->curBBlock->CoInst.push_back(sysy::RVInst("addi", field1, field2, std::to_string(imm)));
      return;
    }
    else {
      switch (op) {
        case sysy::Value::kAdd:
          break;
        case sysy::Value::kSub:
          this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
          this->curBBlock->CoInst.push_back(sysy::RVInst("mul", field1, field1, field2));
          break;
        case sysy::Value::kMul:
          this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
          this->curBBlock->CoInst.push_back(sysy::RVInst("mul", field1, field1, field2));
          break;
        case sysy::Value::kDiv:
          this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
          this->curBBlock->CoInst.push_back(sysy::RVInst("div", field1, field1, field2));
          break;
        case sysy::Value::kRem:
          this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, std::to_string(imm)));
          this->curBBlock->CoInst.push_back(sysy::RVInst("rem", field1, field1, field2));
          break;
        default:
          break;
        return;
      }
    }
  }
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
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
        field1 = regManager.intRegs[addrReg].second;
        field2 = destName;
        this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
        auto tmpReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
        field1 = regManager.intRegs[tmpReg].second;
        field2 = std::to_string(constSrc->getInt());
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
        field1 = regManager.intRegs[tmpReg].second;
        field2 = "0(" + regManager.intRegs[addrReg].second + ")";
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
      // std::cout << destName << destPos->second.second << std::endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
    }
    else if (destPos->second.first == RegisterManager::VarPos::Globals) {
      auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
      field1 = regManager.intRegs[addrReg].second;
      field2 = destName;
      this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
      field1 = regManager.intRegs[srcPos.second].second;
      field2 = "0(" + regManager.intRegs[addrReg].second + ")";
      this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
      regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
    }
    return;
  }
  // case 3: store %b(src, in reg), %a+offset(dst, on stack or in global); %a is an array
  else if (inst->inarray) {
    auto offsetValue = inst->getIndex(0);
    if (src->isConstant()) {
      auto srcImmReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
      auto srcImm = std::to_string(dynamic_cast<sysy::ConstantValue*>(src)->getInt());
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[srcImmReg].second, srcImm));
      if (destPos->second.first == RegisterManager::VarPos::OnStack) {
        if (offsetValue->isConstant()){
          auto offRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
          regManager.releaseReg(RegisterManager::RegType::IntReg, offRegID);
          if (dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt() > -512 && dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt() < 512) {
            field1 = regManager.intRegs[srcImmReg].second;
            field2 = std::to_string(-1*destPos->second.second + 4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(sp)";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
          else {
            field1 = regManager.intRegs[offRegID].second;
            field2 = std::to_string(-1*destPos->second.second + 4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt());
            field3 = "sp";
            this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
            field1 = regManager.intRegs[srcImmReg].second;
            field2 = "0(" + regManager.intRegs[offRegID].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
          return;
        }
        else {
          // offset value is not const
          auto offRegID = regManager.varIRegMap.find(offsetValue->getName())->second;
          assert(offRegID.first == RegisterManager::VarPos::InIReg);
          field1 = regManager.intRegs[offRegID.second].second;
          field2 = regManager.intRegs[offRegID.second].second;
          field3 = "sp";
          this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
          field1 = regManager.intRegs[srcImmReg].second;
          field2 = "0(" + regManager.intRegs[offRegID.second].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          return;
        }
      }
      else if (destPos->second.first == RegisterManager::VarPos::Globals) {
        if (offsetValue->isConstant()) {
          // offset Value is const
          auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
          regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
          field1 = regManager.intRegs[addrReg].second;
          field2 = destName;
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
          field1 = regManager.intRegs[srcImmReg].second;
          int constInt = dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt();
          if (constInt < 512 && constInt > -512) {
            field2 = std::to_string(4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(" + regManager.intRegs[addrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
          else {
            auto addrOffReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
            regManager.releaseReg(RegisterManager::RegType::IntReg, addrOffReg);
            this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[addrOffReg].second, std::to_string(4*constInt)));
            field1 = regManager.intRegs[addrReg].second;
            field2 = regManager.intRegs[addrOffReg].second;
            this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field1, field2));
            field1 = regManager.intRegs[srcImmReg].second;
            field2 = "0(" + regManager.intRegs[addrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
        }
        else {
          // offsetValue is not const
          auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
          regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
          field1 = regManager.intRegs[addrReg].second;
          field2 = destName;
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
          auto offRegID = regManager.varIRegMap.find(offsetValue->getName())->second.second;
          field1 = regManager.intRegs[offRegID].second;
          field2 = regManager.intRegs[offRegID].second;
          field3 = "2";
          this->curBBlock->CoInst.push_back(sysy::RVInst("slli", field1, field2, field3));
          field1 = regManager.intRegs[addrReg].second;
          field2 = regManager.intRegs[addrReg].second;
          field3 = regManager.intRegs[offRegID].second;
          this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
          field1 = regManager.intRegs[srcImmReg].second;
          field2 = "0(" + regManager.intRegs[addrReg].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        }
        regManager.releaseReg(RegisterManager::RegType::IntReg, srcImmReg);
      }
      return;
    }
    else {
      // src is var 
      auto srcPos = regManager.varIRegMap.find(src->getName())->second;
      assert(srcPos.first == RegisterManager::VarPos::InIReg);
      if (offsetValue->isConstant()) {
        // src is var and offsetvalue is const
        if (destPos->second.first == RegisterManager::VarPos::OnStack) {
          field1 = regManager.intRegs[srcPos.second].second;
          field2 = std::to_string(-1*destPos->second.second + 4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(sp)";
          this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        }
        else if (destPos->second.first == RegisterManager::VarPos::Globals) {
          auto addrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, -1);
          field1 = regManager.intRegs[addrReg].second;
          field2 = destName;
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
          auto immNum = dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt();
          if (immNum < 512 && immNum > -512) {
            field1 = regManager.intRegs[srcPos.second].second;
            field2 = std::to_string(4*dynamic_cast<sysy::ConstantValue*>(offsetValue)->getInt()) + "(" + regManager.intRegs[addrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
          else {
            auto immTempReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
            regManager.releaseReg(RegisterManager::RegType::IntReg, immTempReg);
            field1 = regManager.intRegs[immTempReg].second;
            field2 = std::to_string(4*immNum);
            this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
            field1 = regManager.intRegs[immTempReg].second;
            field2 = "0(" + regManager.intRegs[addrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
          }
          regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
        }
        return;
      }
      else {
        // src is var and offsetvalue is var
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
          field2 = "0(" + regManager.intRegs[addrReg].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        }
        else if (destPos->second.first == RegisterManager::VarPos::Globals) {
          field1 = regManager.intRegs[addrReg].second;
          field2 = destName;
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
          field1 = regManager.intRegs[addrReg].second;
          field2 = regManager.intRegs[addrReg].second;
          field3 = regManager.intRegs[regManager.varIRegMap.find(offsetValue->getName())->second.second].second;
          this->curBBlock->CoInst.push_back(sysy::RVInst("slli", field3, field3, "2"));
          this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
          field1 = regManager.intRegs[srcPos.second].second;
          field2 = "0(" + regManager.intRegs[addrReg].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("sw", field1, field2));
        }
        regManager.releaseReg(RegisterManager::RegType::IntReg, addrReg);
        return;
      }
    }
  }
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
      destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::arg, inst->last_used);
      // std::cout << inst->last_used << std::endl;
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
      if (srcPos->second.first == RegisterManager::VarPos::OnStack) {
        field1 = regManager.intRegs[destRegID].second;
        field2 = std::to_string(-1*srcPos->second.second) + "(sp)";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
      }
      else {
        assert(srcPos->second.first == RegisterManager::VarPos::Globals);
        auto tempAddrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
        regManager.releaseReg(RegisterManager::RegType::IntReg, tempAddrReg);
        field1 = regManager.intRegs[tempAddrReg].second;
        field2 = src->getName();
        this->curBBlock->CoInst.push_back(sysy::RVInst("la", field1, field2));
        field1 = regManager.intRegs[destRegID].second;
        field2 = "0(" + regManager.intRegs[tempAddrReg].second + ")";
        this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2)); 
      }
    }
    // case 2: load %v (dst, in reg), %arr+%t(array, on stack or in global)
    else {
      destRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
      regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, destRegID};
      auto offset = inst->getIndex(0);
      if (srcPos->second.first == RegisterManager::VarPos::OnStack) {
        if (offset->isConstant()) {
          field1 = regManager.intRegs[destRegID].second;
          field2 = std::to_string(-1*srcPos->second.second + 4*dynamic_cast<sysy::ConstantValue*>(offset)->getInt()) + "(sp)";
          this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
        }
        else {
          assert(regManager.varIRegMap.find(offset->getName())->second.first == RegisterManager::VarPos::InIReg);
          field1 = regManager.intRegs[destRegID].second;
          field2 = regManager.intRegs[regManager.varIRegMap.find(offset->getName())->second.second].second;
          field3 = "sp";
          this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
          field1 = regManager.intRegs[destRegID].second;
          field2 = "0(" + regManager.intRegs[destRegID].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
        }
      }
      else if (srcPos->second.first == RegisterManager::VarPos::Globals) {
        auto tempAddrReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, -1);
        if (offset->isConstant()) {
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", regManager.intRegs[tempAddrReg].second, src->getName()));
          auto constOff = dynamic_cast<sysy::ConstantValue*>(offset)->getInt();
          if (constOff < 512 && constOff > -512) {
            field1 = regManager.intRegs[destRegID].second;
            field2 = std::to_string(4*constOff) + "(" + regManager.intRegs[tempAddrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
          }
          else {
            // imm to large
            auto reqOffsetImmReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
            regManager.releaseReg(RegisterManager::RegType::IntReg, reqOffsetImmReg);
            field1 = regManager.intRegs[reqOffsetImmReg].second;
            field2 = std::to_string(4*constOff);
            this->curBBlock->CoInst.push_back(sysy::RVInst("li", field1, field2));
            field1 = regManager.intRegs[tempAddrReg].second;
            field2 = regManager.intRegs[tempAddrReg].second;
            field3 = regManager.intRegs[reqOffsetImmReg].second;
            this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
            field1 = regManager.intRegs[destRegID].second;
            field2 = "0(" + regManager.intRegs[tempAddrReg].second + ")";
            this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
          }
        }
        else {
          // off is not imm
          this->curBBlock->CoInst.push_back(sysy::RVInst("la", regManager.intRegs[tempAddrReg].second, src->getName()));
          assert(regManager.varIRegMap.find(offset->getName())->second.first == RegisterManager::VarPos::InIReg);
          field1 = regManager.intRegs[tempAddrReg].second;
          field2 = regManager.intRegs[tempAddrReg].second;
          field3 = regManager.intRegs[regManager.varIRegMap.find(offset->getName())->second.second].second;
          this->curBBlock->CoInst.push_back(sysy::RVInst("slli", field3, field3, "2"));
          this->curBBlock->CoInst.push_back(sysy::RVInst("add", field1, field2, field3));
          field1 = regManager.intRegs[destRegID].second;
          field2 = "0(" + regManager.intRegs[tempAddrReg].second + ")";
          this->curBBlock->CoInst.push_back(sysy::RVInst("lw", field1, field2));
          return;
        }
        regManager.releaseReg(RegisterManager::RegType::IntReg, tempAddrReg);
      }
      else { assert(0); }
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
  if (cond->isConstant()) {
    if (dynamic_cast<sysy::ConstantValue*>(cond)->getInt() == 0) {
      this->curBBlock->CoInst.push_back(sysy::RVInst("j", this->getBasicBlocksLabel(inst->getElseBlock())));
    }
    else {
      this->curBBlock->CoInst.push_back(sysy::RVInst("j", this->getBasicBlocksLabel(inst->getThenBlock())));
    }
    return;
  }
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
    this->curBBlock->CoInst.push_back(sysy::RVInst("bnez", field1, field2));
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
  auto dstRegID = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::RegHint::temp, inst->last_used);
  regManager.varIRegMap[inst->getName()] = {RegisterManager::VarPos::InIReg, dstRegID};
  // std::cout << "XXX" << inst->getName() << std::endl;
  if (op == sysy::Value::Kind::kNeg) {
    if (inst->getOperand()->isConstant()) {
      this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[dstRegID].second, std::to_string(dynamic_cast<sysy::ConstantValue *>(inst->getOperand())->getInt()*-1)));
    }
    else {
      field1 = regManager.intRegs[dstRegID].second;
      field2 = regManager.intRegs[regManager.varIRegMap.find(inst->getOperand()->getName())->second.second].second;
      // instruction += space + "neg " + field1 + ", " + field2 + endl;
      this->curBBlock->CoInst.push_back(sysy::RVInst("neg", field1, field2));
    }
    return ;
  }
  else if (op == sysy::Value::Kind::kNot) {
    if (inst->getOperand()->isConstant()) {
      if (dynamic_cast<sysy::ConstantValue *>(inst->getOperand())->getInt() == 0) {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[dstRegID].second, std::to_string(1)));
      }
      else {
        this->curBBlock->CoInst.push_back(sysy::RVInst("li", regManager.intRegs[dstRegID].second, std::to_string(0)));
      }
    }
    else {
      field1 = regManager.intRegs[dstRegID].second;
      field2 = regManager.intRegs[regManager.varIRegMap.find(inst->getOperand()->getName())->second.second].second;;
      this->curBBlock->CoInst.push_back(sysy::RVInst("seqz", field1, field2));
    }
    return ;
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
      auto requestReg = regManager.requestReg(RegisterManager::RegType::IntReg, RegisterManager::arg, inst->last_used);
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
    case sysy::Value::Kind::kRem:
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
    case sysy::Value::Kind::kAnd:
    case sysy::Value::Kind::kICmpNE:
    case sysy::Value::Kind::kOr:
    case sysy::Value::Kind::kICmpGE:
    case sysy::Value::Kind::kICmpLE:
      GenBinaryCmpInst(dynamic_cast<sysy::BinaryInst *>(inst));
      break;
    case sysy::Value::Kind::kNeg:
    case sysy::Value::Kind::kNot:
      GenUnaryInst(dynamic_cast<sysy::UnaryInst *>(inst));
      break;
    case sysy::Value::Kind::kCall:
      GenCallInst(dynamic_cast<sysy::CallInst *>(inst), 10);
      break;
    default:
      std::cerr << "Error: invalid instruction" << std::endl;
      assert(0);
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
      return this->requestReg(RegType::IntReg, RegHint::saved, last_used);
      return 0;
    case saved:
      for (auto i: this->IsavedRegList) {
        if (this->intRegTaken[i].first == false) {
          this->intRegTaken[i].first = true;
          this->intRegTaken[i].second = last_used;
          return i;
        }
      }
      return this->requestReg(RegType::IntReg, RegHint::arg, last_used);
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