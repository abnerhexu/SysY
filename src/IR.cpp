#include "IR.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace sysy {

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

void User::setOperand(int index, Value *value) {
  assert(index < getNumOperands());
  operands[index].setValue(value);
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

Function *CallInst::getCallee() {
  return dynamic_cast<Function *>(getOperand(0));
}

} // namespace sysy