#pragma once

#include "IR.h"
#include <cassert>
#include <memory>

namespace sysy {

class IRBuilder {
private:
  BasicBlock *block;
  BasicBlock::iterator position;

public:
  IRBuilder() = default;
  IRBuilder(BasicBlock *block) : block(block), position(block->end()) {}
  IRBuilder(BasicBlock *block, BasicBlock::iterator position)
      : block(block), position(position) {}

public:
  BasicBlock *getBasicBlock() const { return block; }
  BasicBlock::iterator getPosition() const { return position; }
  void setPosition(BasicBlock *block, BasicBlock::iterator position) {
    this->block = block;
    this->position = position;
  }
  void setPosition(BasicBlock::iterator position) { this->position = position; }

public:
  UnaryInst *createUnaryInst(Instruction::Kind kind, Type *type, Value *operand,
                             const std::string &name = "") {

    auto inst = new UnaryInst(kind, type, operand, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  UnaryInst *createNegInst(Value *operand, const std::string &name = "") {
    return createUnaryInst(Instruction::kNeg, Type::getIntType(), operand,
                           name);
  }
  UnaryInst *createNotInst(Value *operand, const std::string &name = "") {
    return createUnaryInst(Instruction::kNot, Type::getIntType(), operand,
                           name);
  }
  UnaryInst *createFtoIInst(Value *operand, const std::string &name = "") {
    return createUnaryInst(Instruction::kFtoI, Type::getIntType(), operand,
                           name);
  }
  UnaryInst *createFNegInst(Value *operand, const std::string &name = "") {
    return createUnaryInst(Instruction::kFNeg, Type::getFloatType(), operand,
                           name);
  }
  UnaryInst *createIToFInst(Value *operand, const std::string &name = "") {
    return createUnaryInst(Instruction::kIToF, Type::getFloatType(), operand,
                           name);
  }
  BinaryInst *createBinaryInst(Instruction::Kind kind, Type *type, Value *lhs,
                               Value *rhs, const std::string &name = "") {
    auto inst = new BinaryInst(kind, type, lhs, rhs, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  BinaryInst *createAddInst(Value *lhs, Value *rhs,
                            const std::string &name = "") {
    return createBinaryInst(Instruction::kAdd, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createSubInst(Value *lhs, Value *rhs,
                            const std::string &name = "") {
    return createBinaryInst(Instruction::kSub, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createMulInst(Value *lhs, Value *rhs,
                            const std::string &name = "") {
    return createBinaryInst(Instruction::kMul, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createDivInst(Value *lhs, Value *rhs,
                            const std::string &name = "") {
    return createBinaryInst(Instruction::kDiv, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createRemInst(Value *lhs, Value *rhs,
                            const std::string &name = "") {
    return createBinaryInst(Instruction::kRem, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpEQInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpEQ, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpNEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpNE, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpLTInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpLT, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpLEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpLE, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpGTInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpGT, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createICmpGEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kICmpGE, Type::getIntType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFAddInst(Value *lhs, Value *rhs,
                             const std::string &name = "") {
    return createBinaryInst(Instruction::kFAdd, Type::getFloatType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFSubInst(Value *lhs, Value *rhs,
                             const std::string &name = "") {
    return createBinaryInst(Instruction::kFSub, Type::getFloatType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFMulInst(Value *lhs, Value *rhs,
                             const std::string &name = "") {
    return createBinaryInst(Instruction::kFMul, Type::getFloatType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFDivInst(Value *lhs, Value *rhs,
                             const std::string &name = "") {
    return createBinaryInst(Instruction::kFDiv, Type::getFloatType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFRemInst(Value *lhs, Value *rhs,
                             const std::string &name = "") {
    return createBinaryInst(Instruction::kFRem, Type::getFloatType(), lhs, rhs,
                            name);
  }
  BinaryInst *createFCmpEQInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpEQ, Type::getFloatType(), lhs,
                            rhs, name);
  }
  BinaryInst *createFCmpNEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpNE, Type::getFloatType(), lhs,
                            rhs, name);
  }
  BinaryInst *createFCmpLTInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpLT, Type::getFloatType(), lhs,
                            rhs, name);
  }
  BinaryInst *createFCmpLEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpLE, Type::getFloatType(), lhs,
                            rhs, name);
  }
  BinaryInst *createFCmpGTInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpGT, Type::getFloatType(), lhs,
                            rhs, name);
  }
  BinaryInst *createFCmpGEInst(Value *lhs, Value *rhs,
                               const std::string &name = "") {
    return createBinaryInst(Instruction::kFCmpGE, Type::getFloatType(), lhs,
                            rhs, name);
  }
  ReturnInst *createReturnInst(Value *value = nullptr) {
    auto inst = new ReturnInst(value);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  UncondBrInst *createUncondBrInst(BasicBlock *block,
                                   std::vector<Value *> args) {
    auto inst = new UncondBrInst(block, args, block);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  CondBrInst *createCondBrInst(Value *condition, BasicBlock *thenBlock,
                               BasicBlock *elseBlock,
                               const std::vector<Value *> &thenArgs,
                               const std::vector<Value *> &elseArgs) {
    auto inst = new CondBrInst(condition, thenBlock, elseBlock, thenArgs,
                               elseArgs, block);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  AllocaInst *createAllocaInst(Type *type,
                               const std::vector<Value *> &dims = {},
                               const std::string &name = "") {
    auto inst = new AllocaInst(type, dims, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  LoadInst *createLoadInst(Value *pointer,
                           const std::vector<Value *> &indices = {},
                           const std::string &name = "") {
    auto inst = new LoadInst(pointer, indices, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  StoreInst *createStoreInst(Value *value, Value *pointer,
                             const std::vector<Value *> &indices = {},
                             const std::string &name = "") {
    auto inst = new StoreInst(value, pointer, indices, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
};

} // namespace sysy