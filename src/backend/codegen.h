#ifndef __codegen__
#define __codegen__
#include <iostream>
#include <vector>
#include "../frontend/IR.h"

namespace codegen {
static const std::string space = "  ";
static const std::string endl = "\n";

static std::string RTypeInst(std::string name, std::string rd, std::string rs1, std::string rs2) {
  return space + name + " " + rd + ", " + rs1 + ", " + rs2;
}

class RegisterManager {
  friend class CodeGen;
  friend class LLIRGen;
public:
  std::vector<std::pair<std::string, std::string>> intRegs = {{"x0", "zero"}, {"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"}, {"x4", "tp"}, {"x5", "t0"}, {"x6", "t1"}, {"x7", "t2"}, {"x8", "s0"}, {"x9", "s1"}, {"x10", "a0"}, {"x11", "a1"}, {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"}, {"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"}, {"x20", "s4"}, {"x21", "s5"}, 
  {"x22", "s6"}, {"x23", "s7"}, {"x24", "s8"}, {"x25", "s9"}, {"x26", "s10"}, {"x27", "s11"}, {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"}, {"x31", "t6"}};

  std::vector<std::pair<std::string, std::string>> floatRegs = {{"f0", "ft0"}, {"f1", "ft1"}, {"f2", "ft2"}, {"f3", "ft3"}, {"f4", "ft4"}, {"f5", "ft5"}, {"f6", "ft6"}, {"f7", "ft7"}, {"f8", "fs0"}, {"f9", "fs1"}, {"f10", "fa0"}, {"f11", "fa1"}, {"f12", "fa2"}, {"f13", "fa3"}, {"f14", "fa4"}, {"f15", "fa5"}, {"f16", "fa6"}, {"f17", "fa7"}, {"f18", "fs2"}, {"f19", "fs3"}, {"f20", "fs4"}, {"f21", "fs5"}, {"f22", "fs6"},
  {"f23", "fs7"}, {"f24", "fs8"}, {"f25", "fs9"}, {"f26", "fs10"}, {"f27", "fs11"}, {"f28", "ft8"}, {"f29", "ft9"}, {"f30", "ft10"}, {"f31", "ft11"}};

  enum VarPos {
    Globals, OnStack, InIReg, InFReg, Imm
  };
  // string: name
  // varPos: indication the var position
  // int: stack offset (if on stack), or reg id (if on regs)
  std::map<std::string, std::pair<VarPos, int>> varIRegMap;
  std::map<std::string, std::pair<VarPos, int>> varFRegMap;

  std::vector<int> ItempRegList = {6, 7, 28, 29, 30, 31};
  std::vector<int> IsavedRegList = {9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
  std::vector<int> IdontCareRegList = {5, 6, 7, 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  std::vector<int> IArgRegList = {10, 11, 12, 13, 14, 15, 16, 17};

  std::map<sysy::Function*, int> spOffset;
  // int: allocated fake register, 
  // pair.first: lastly used inst_index, 
  // pair.second: the inst itself
  std::map<int, std::pair<int, sysy::Instruction*>> LastVisit; 
private:
  // first: taken or not
  // second: last used
  std::vector<std::pair<bool, int>> intRegTaken;
  std::vector<std::pair<bool, int>> floatRegTaken;

public:
  enum RegType {
    IntReg, FloatReg
  };

  enum RegHint {
    dontCare,
    zero,
    ra,
    sp,
    gp,
    tp,
    temp,
    fp,
    arg,
    saved
  };

public:
  int requestReg(RegType rtype, RegHint hint = dontCare, int last_used = 0);
  bool releaseReg(RegType rtype, int regID) {
    if (rtype == IntReg) {
      this->intRegTaken[regID].first = false;
    }
    else if (rtype == FloatReg) {
      this->floatRegTaken[regID].first = false;
    }
    return true;
  }

  void resetRegs(RegType rtype){
    switch (rtype) {
    case IntReg:
      this->intRegTaken.clear();
      this->intRegTaken.resize(32, {false, 0});
      break;
    case FloatReg:
      this->floatRegTaken.clear();
      this->floatRegTaken.resize(32, {false, 0});
      break;
    default:
      std::cerr << "Unknown register type" << std::endl;
      exit(1);
    }
  }

  void gc(int inst_index);

public:
  RegisterManager() {
    this->resetRegs(IntReg);
    this->resetRegs(FloatReg);
  }
}; // class RegisterManager

extern RegisterManager regManager;

class InstOperand {
public:
  enum Kind {
    IReg, Imm, Freg
  };
  Kind kind;
  int imm;
  int iregID;
  int fregID;
  InstOperand() {};
  InstOperand(int x, Kind kind) {
    this->kind = kind;
    if (kind == IReg) {
      this->iregID = x;
    }
    else if (kind == Freg) {
      this->fregID = x;
    }
    else if (kind == Imm) {
      this->imm = x;
    }
    else {
      std::cerr << "Unknown operand kind" << std::endl;
      exit(1);
    }
  }

  int getImm() {
    if (this->kind == Imm) {
      return this->imm;
    }
    else {
      std::cerr << "Not an immediate operand" << std::endl;
      exit(1);
    }
  }

  int getIregID() {
    if (this->kind == IReg) {
      return this->iregID;
    }
    else {
      std::cerr << "Not an integer register operand" << std::endl;
      exit(1);
    }
  }

  int getFregID() {
    if (this->kind == Freg) {
      return this->fregID;
    }
    else {
      std::cerr << "Not a floating point register operand" << std::endl;
      exit(1);
    }
  }
}; // class InstOperand


class CodeGen {
public:
  enum Kind {
    IReg, Imm, Freg
  };
  Kind kind;
  std::string fname;
private:
  sysy::Module* module;
  sysy::Function* curFunc;
  sysy::BasicBlock* curBBlock;
  RegisterManager rmanager;
  // std::map<std::string, std::vector<RVInst>> CoInsts;

  // global values
  bool loadGlobalValue = true;

  // basic block
  std::vector<sysy::BasicBlock*> bblocks;
  int bblockId = 0;

  // function params, return value and localValues
  std::map<sysy::Argument*, int> paramsOffset;
  std::map<sysy::Instruction*, int> localVarOffset;
  int retValueOffset = 0;
  // TODO 
  // int OffsetAcc = 0;
  // label manager
  std::map<sysy::BasicBlock*, std::string> bblockLabels;
  int bblabelId = 0;
  int GAccessBB = 0;

public:
  CodeGen(sysy::Module* module, const std::string fname = ""): module(module), fname(fname) {};
  void code_gen();
  void module_gen(sysy::Module* module);
  void function_gen(sysy::Function* func);
  void basicBlock_gen(sysy::BasicBlock* bblock);
  void instruction_gen(sysy::Instruction* inst);
  std::string globalData_gen(sysy::Module* module);
  void CalleeRegSave_gen(sysy::Function *func);
  void CalleeRegRestore_gen(sysy::Function *func);
  void literalPoolsCode_gen(sysy::Function *func);
  void functionHeader_gen(sysy::Function *func);
  std::string GAccessBB_gen() {
    GAccessBB++;
    return ".Lpcrel_hi" + std::to_string(this->GAccessBB);
  }

  void clearModuleLabels(sysy::Module *module) {
    this->bblabelId = 0;
  }

  // instruction generator
  void GenLoadInst(sysy::LoadInst* inst);
  void GenStoreInst(sysy::StoreInst* inst);
  void GenAllocaInst(sysy::AllocaInst* inst);
  void GenReturnInst(sysy::ReturnInst* inst);
  void GenCallInst(sysy::CallInst* inst, int dstRegID);
  void GenBinaryInst(sysy::BinaryInst* inst);
  void GenBinaryCmpInst(sysy::BinaryInst *inst);
  // std::pair<int, std::string> GenUnaryInst(sysy::UnaryInst* inst, int dstRegID);
  void GenUncondBrInst(sysy::UncondBrInst* inst);
  void GenCondBrInst(sysy::CondBrInst* inst);
  void GenUnaryInst(sysy::UnaryInst* inst);

  std::string SaveReg2Stack(int regID, Kind kind, sysy::Instruction* inst);
  void clearFuncInfo(sysy::Function *func) {
    this->localVarOffset.clear();
    this->paramsOffset.clear();
    this->retValueOffset = 0;
    this->bblockLabels.clear();
    //TODO this->OffsetAcc = 0;
  }

  std::string getBasicBlocksLabel(sysy::BasicBlock* bb) {
    auto t = this->bblockLabels.find(bb);
    std::string label;
    if (t == this->bblockLabels.end()) {
      // label = ".block_" + std::to_string(this->bblabelId++); // + "_" + bb->getName();
      label = "BB_" + std::to_string(this->bblabelId++); // bb->getName().;
      bblockLabels[bb] = label;
    }
    else {
      label = t->second;
    }
    return label;
  }
}; // class CodeGen


}

#endif