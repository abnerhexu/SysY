#include "IR.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "SysYIRGenerator.h"

namespace sysy {

template <typename T>
std::ostream &interleave(std::ostream &os, const T &container, const std::string sep = ", ") {
  if (!container.empty()) {
    for (auto it = container.begin(); it != container.end(); it = std::next(it)) {
      os << *it << (std::next(it) == container.end() ? "" : sep);
    }
    return os;
  }
  return os;
}

static inline std::ostream &printVarName(std::ostream &os, const Value *var) {
  return os << (dynamicCast<GlobalValue>(var) ? '@' : '%') << var->getName();
}
static inline std::ostream &printBlockName(std::ostream &os, const BasicBlock *block) {
  return os << block->getName() << ":";
}
static inline std::ostream &printFunctionName(std::ostream &os, const Function *func) {
  return os << '@' << func->getName();
}
static inline std::ostream &printOperand(std::ostream &os, const Value *value) {
  auto constant = dynamicCast<ConstantValue>(value);
  if (constant) {
    constant->print(os);
    return os;
  }
  return printVarName(os, value);
}
//===----------------------------------------------------------------------===//
// Types
//===----------------------------------------------------------------------===//

Type *Type::getIntType() {
  static Type intType(kInt);
  return &intType;
}

Type *Type::getFloatType() {
  static Type floatType(kFloat);
  return &floatType;
}

Type *Type::getVoidType() {
  static Type voidType(kVoid);
  return &voidType;
}

Type *Type::getLabelType() {
  static Type labelType(kLabel);
  return &labelType;
}

Type *Type::getPointerType(Type *baseType) {
  // forward to PointerType
  return PointerType::get(baseType);
}

Type *Type::getFunctionType(Type *returnType,
                            const std::vector<Type *> &paramTypes) {
  // forward to FunctionType
  return FunctionType::get(returnType, paramTypes);
}

int Type::getSize() const {
  switch (kind) {
  case kInt:
  case kFloat:
    return 4;
  case kLabel:
  case kPointer:
  case kFunction:
    return 8;
  case kVoid:
    return 0;
  }
  return 0;
}

void Type::print(std::ostream &os) const {
  auto kind = this->getKind();
  std::vector<std::string> typs = {"i32", "float", "void"};
  if (kInt <= kind && kind <= kVoid) {
    os << typs[kind];
  }
  else if (kind == kPointer) {
    static_cast<const PointerType *>(this)->getBaseType()->print(os);
    os << "*";
  }
  else if (kind == kFunction) {
    static_cast<const FunctionType *>(this)->getReturnType()->print(os);
    os << "(";
    interleave(os, static_cast<const FunctionType*>(this)->getParamTypes());
    os << ")";
  }
  else {
    os << "Unsupported type!" << std::endl;
  }
}

PointerType *PointerType::get(Type *baseType) {
  static std::map<Type *, std::unique_ptr<PointerType>> pointerTypes;
  auto iter = pointerTypes.find(baseType);
  if (iter != pointerTypes.end())
    return iter->second.get();
  auto type = new PointerType(baseType);
  assert(type);
  auto result = pointerTypes.emplace(baseType, type);
  return result.first->second.get();
}

FunctionType *FunctionType::get(Type *returnType,
                                const std::vector<Type *> &paramTypes) {
  static std::set<std::unique_ptr<FunctionType>> functionTypes;
  auto iter =
      std::find_if(functionTypes.begin(), functionTypes.end(),
                   [&](const std::unique_ptr<FunctionType> &type) -> bool {
                     if (returnType != type->getReturnType() or
                         paramTypes.size() != type->getParamTypes().size())
                       return false;
                     return std::equal(paramTypes.begin(), paramTypes.end(),
                                       type->getParamTypes().begin());
                   });
  if (iter != functionTypes.end())
    return iter->get();
  auto type = new FunctionType(returnType, paramTypes);
  assert(type);
  auto result = functionTypes.emplace(type);
  return result.first->get();
}

void Value::replaceAllUsesWith(Value *value) {
  for (auto &use : uses)
    use->getUser()->setOperand(use->getIndex(), value);
  uses.clear();
}

bool Value::isConstant() const {
  if (dynamicCast<GlobalValue>(this) or dynamicCast<Function>(this) or dynamicCast<ConstantValue>(this)) {
    return true;
  }
  return false;
}

ConstantValue *ConstantValue::get(int value, const std::string &name) {
  static std::map<int, std::unique_ptr<ConstantValue>> intConstants;
  auto iter = intConstants.find(value);
  if (iter != intConstants.end())
    return iter->second.get();
  auto inst = new ConstantValue(value);
  assert(inst);
  auto result = intConstants.emplace(value, inst);
  return result.first->second.get();
}

ConstantValue *ConstantValue::get(float value, const std::string &name) {
  static std::map<float, std::unique_ptr<ConstantValue>> floatConstants;
  auto iter = floatConstants.find(value);
  if (iter != floatConstants.end())
    return iter->second.get();
  auto inst = new ConstantValue(value);
  assert(inst);
  auto result = floatConstants.emplace(value, inst);
  return result.first->second.get();
}

void ConstantValue::print(std::ostream &os) const {
  if (isInt()) {
    os << getInt();
  }
  else {
    os << getFloat();
  }
}

// see IR.h Argument class
// Argument::Argument(Type *type, BasicBlock *block, int index, const std::string &name) : Value(kArgument, type, name), block(block), index(index) {
//   if (not hasName()) {
//     setName(std::to_string(block->getParent()->allocateVariableID()));
//   }
// }

Argument::Argument(Type *type, BasicBlock *block, int index, const std::string &name) : Value(kArgument, type, name), block(block), index(index) {
    if (not hasName()) {
      setName(std::to_string(block->getParent()->allocateVariableID()));
    }
}

void Argument::print(std::ostream &os) const {
  assert(hasName());
  printVarName(os, this) << ": " << this->getType(); //TODO
}

// basic block construct function: see IR.h, line 360

BasicBlock::BasicBlock(Function *parent, const std::string &name) : Value(kBasicBlock, Type::getLabelType(), name), parent(parent),
      instructions(), arguments(), successors(), predecessors() {
  if (not hasName())
    setName("bb" + std::to_string(getParent()->allocateblockID()));
}

void BasicBlock::print(std::ostream &os) {
  assert(hasName());
  os << "  ";
  printBlockName(os, this);
  auto args = this->getArguments();
  if (not args.empty()) {
    os << "(";
    for (auto arg = args.begin(); arg != args.end(); arg = std::next(arg)) {
      printVarName(os, arg->get()) << ": ";
      arg->get()->getType()->print(os);
      os << (std::next(arg) == args.end()? "": ", ");
    }
    os << ")";
  }
  os << std::endl;
  for (auto &inst: this->instructions) {
    os << "    " << *inst << std::endl;
  }
}

// Instruction construct function: see IR.h, line 495

Instruction::Instruction(Kind kind, Type *type, BasicBlock *parent,
              const std::string &name): User(kind, type, name), kind(kind), parent(parent) {
  if (not type->isVoid() and not hasName())
    setName("v" + std::to_string(getFunction()->allocateVariableID()));
}

void CallInst::print(std::ostream &os) const {
  if (not this->getType()->isVoid()) {
    printVarName(os, this) << " = call ";
  }
  printFunctionName(os, getCallee()) << "(";
  auto args = getArguments();
  if (not args.empty()) {
    for (auto arg = args.begin(); arg != args.end(); arg = std::next(arg)) {
      printOperand(os, arg->getValue());
      os << (std::next(arg) == args.end()? "": ", ");
    }
  }
  os << ") : " << *getType();
}

void UnaryInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  std::vector<std::string> ops = {"neg", "not", "fneg", "ftoi", "itof"};
  auto kind = this->getKind();
  if (kind < kNeg or kind > kItoF){
    assert(false);
  }
  else {
    os << ops[kind - kNeg] << " ";
  }
  printOperand(os, getOperand()) << " : ";
  this->getType()->print(os);
}

void BinaryInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  auto kind = this->getKind();
  std::vector<std::string> ops = {"add", "sub", "mul", "div", "rem", "icmpeq", "icmpne", "icmplt", "icmpgt", "icmple", "icmpge", "fadd", "fsub", "fmul", "fdiv", "frem", "fcmpeq", "fcmpne", "fcmplt", "fcmpgt", "fcmple", "fcmpge", "or", "and", "for", "fand", "sll"};
  if (kind < kAdd or kind > kSLL) {
    assert(false);
  }
  else {
    os << ops[kind - kAdd] << " ";
  }
  printOperand(os, this->getLhs()) << ", ";
  printOperand(os, this->getRhs()) << " : ";
  this->getType()->print(os);
}

void ReturnInst::print(std::ostream &os) const {
  os << "return";
  auto rvalue = this->getReturnValue();
  if (rvalue != nullptr) {
    os << " ";
    printOperand(os, rvalue) << " : ";
    rvalue->getType()->print(os);
  }
}

void UncondBrInst::print(std::ostream &os) const {
  os << "br " << this->getBlock()->getName();
  auto args = this->getArguments();
  if (!args.empty()) {
    os << "(";
    for (auto arg = args.begin(); arg != args.end(); arg = std::next(arg)) {
      printOperand(os, arg->getValue());
      if (std::next(arg) != args.end()) {
        os << ", ";
      }
    }
    os << ")";
  }
}

void CondBrInst::print(std::ostream &os) const {
  os << "condbr ";
  printOperand(os, this->getCondition()) << ", " << this->getThenBlock()->getName() << ", ";
  // then block
  auto args = this->getThenArguments();
  if (not args.empty()) {
    os << "(";
    for (auto arg = args.begin(); arg != args.end(); arg = std::next(arg)) {
      printOperand(os, arg->getValue());
      if (std::next(arg) != args.end()) {
        os << ", ";
      }
    }
    os << ")";
  }
  os << this->getElseBlock()->getName();
  // else block
  auto eargs = getElseArguments();
  if (not eargs.empty()) {
    os << "(";
    for (auto earg = eargs.begin(); earg != eargs.end(); earg = std::next(earg)) {
      printOperand(os, earg->getValue());
      if (std::next(earg) != eargs.end()) {
        os << ", ";
      }
    }
    os << ")";
  }
}

void AllocaInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  if (getNumDims()) {
    //std::cerr << "do not support arrays!" << std::endl;
    os << "alloca ";
    for (int i = 0; i < getNumDims(); i++){
      //std::cout << *getDim(i) << ' ';
      os << "[" << *getDim(i) << " x ";
    }
    os << *static_cast<const PointerType *>(getType())->getBaseType();
    for (int i = 0; i < getNumDims(); i++){
      //std::cout << *getDim(i) << ' ';
      os << "]";
    }
    os << " : " << *getType();
  }else{
    os << "alloca " << *static_cast<const PointerType *>(getType())->getBaseType() << " : " << *getType();
  }
}

void LoadInst::print(std::ostream &os) const {
  printVarName(os, this) << " = ";
  os << "load ";
  if (getNumIndices()) {
    // std::cerr << "do not support arrays!" << std::endl;
    int len = 1;
    int offset = 0;
    std::string name = getPointer()->getName();
    for (int i = getNumIndices()-1; i >= 0; i--){
      offset += (dynamicCast<ConstantValue>(getIndex(i)))->getInt() * len;
      len *= (dynamicCast<ConstantValue>(usedarrays[name][i]))->getInt();
    }
    printOperand(os, getPointer()) << "+" << offset << "(" << *getPointer()->getType() << ") : " << *getType();
  }else{
    printOperand(os, getPointer()) << " : " << *getType();
  }
}

void StoreInst::print(std::ostream &os) const {
  os << "store ";
  printOperand(os, getValue()) << ", ";
  // std::cout << getNumIndices() << std::endl;
  if (getNumIndices()) {
    // assignment to arrays
    std::string name = getPointer()->getName();
    printOperand(os, getPointer());
    for (int i = 0; i < getNumIndices(); i++){
      os << "+";
      // std::cout << *getIndex(i) << ' ';
      // offset += (dynamicCast<ConstantValue*>(getIndex(i)))->getInt() * len;
      // len *= (dynamicCast<ConstantValue*>(usedarrays[name][i]))->getInt();
      // std::cout << (dynamicCast<ConstantValue>(getIndex(i)))->getInt() << std::endl;
      if (getIndex(i)->isConstant()) {
        os << dynamic_cast<ConstantValue*>(getIndex(i))->getInt();
      }
      else {
        // std::cout << "name: " << getIndex(i)->getName() << std::endl;
        os << "%" << getIndex(i)->getName();
      }
      for (int j = i + 1; j < getNumIndices(); j++) {
        os << " * ";
        if (usedarrays[name][j]->isConstant()) { os << dynamic_cast<ConstantValue*>(usedarrays[name][j])->getInt();}
        else {os << usedarrays[name][j]->getName(); }
        // if (j != getNumIndices() - 1) {os << "*"; }
      }
    }
    // std::cout << offset << std::endl;
    os << "(" << *getPointer()->getType() << ") : " << *getValue()->getType();
  }
  else {
    printOperand(os, getPointer()) << " : " << *getValue()->getType();
  }
}

void Function::print(std::ostream &os) const {
  auto returnType = getReturnType();
  auto paramTypes = getParamTypes();
  os << *returnType << " ";
  printFunctionName(os, this) << "(";
  for (auto paramType = paramTypes.begin(); paramType != paramTypes.end(); paramType = std::next(paramType)) {
    (*paramType)->print(os);
    if (std::next(paramType) != paramTypes.end()) {
      os << ", ";
    }
  }
  os << ") {" << std::endl;

  for (auto &bb: this->getBasicBlocks()) {
    bb->print(os);
    os << std::endl;
  }
  os << "}";
}

void Module::print(std::ostream &os) const {
  // std::cout << "module length" << this->children.size() << std::endl;
  for(auto &value: this->children) {
    os << *value << std::endl;
  }
}

void User::setOperand(int index, Value* value) {
  assert(index < this->getNumOperands());
  this->operands[index].setValue(value);
}

void User::replaceOperand(int index, Value *value) {
  assert(index < getNumOperands());
  auto &use = operands[index];
  use.getValue()->removeUse(&use);
  use.setValue(value);
}

CallInst::CallInst(Function *callee, const std::vector<Value *> args,
                          BasicBlock *parent, const std::string &name)
    : Instruction(kCall, callee->getReturnType(), parent, name) {
  addOperand(callee);
  for (auto arg : args)
    addOperand(arg);
}

Function *CallInst::getCallee() const {
  return dynamicCast<Function>(getOperand(0));
}

void GlobalValue::print(std::ostream &os) const {
  os << this->getName() << " global_var ";
  int numDims = this->getNumDims();// if numDims > 0, it is array
  for (int i = 0; i < numDims; i++) {
    os << "[" << dynamicCast<ConstantValue>(this->getDim(i))->getInt() << " x ";
  }
  os << *static_cast<const PointerType *>(getType())->getBaseType();
  for (int i = 0; i < numDims; i++) {
    os << "] ";
  }
  os << ": " << *this->getType();
}

} // namespace sysy