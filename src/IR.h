#pragma once

#include "range.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace sysy {

/*!
 * \defgroup type Types
 * The SysY type system is quite simple.
 * 1. The base class `Type` is used to represent all primitive scalar types,
 * include `int`, `float`, `void`, and the label type representing branch
 * targets.
 * 2. `PointerType` and `FunctionType` derive from `Type` and represent pointer
 * type and function type, respectively.
 *
 * NOTE `Type` and its derived classes have their ctors declared as 'protected'.
 * Users must use Type::getXXXType() methods to obtain `Type` pointers.
 * @{
 */

/*!
 * `Type` is used to represent all primitive scalar types,
 * include `int`, `float`, `void`, and the label type representing branch
 * targets
 */
class Type {
public:
  enum Kind {
    kInt,
    kFloat,
    kVoid,
    kLabel,
    kPointer,
    kFunction,
  };
  Kind kind;

protected:
  Type(Kind kind) : kind(kind) {}
  virtual ~Type() {}

public:
  static Type *getIntType();
  static Type *getFloatType();
  static Type *getVoidType();
  static Type *getLabelType();
  static Type *getPointerType(Type *baseType);
  static Type *getFunctionType(Type *returnType,
                               const std::vector<Type *> &paramTypes = {});

public:
  Kind getKind() const { return kind; }
  bool isInt() const { return kind == kInt; }
  bool isFloat() const { return kind == kFloat; }
  bool isVoid() const { return kind == kVoid; }
  bool isLabel() const { return kind == kLabel; }
  bool isPointer() const { return kind == kPointer; }
  bool isFunction() const { return kind == kFunction; }
  int getSize() const;
  template <typename T>
  std::enable_if_t<std::is_base_of_v<Type, T>, T *> as() const {
    return dynamic_cast<T *>(const_cast<Type *>(this));
  }
}; // class Type

//! Pointer type
class PointerType : public Type {
protected:
  Type *baseType;

protected:
  PointerType(Type *baseType) : Type(kPointer), baseType(baseType) {}

public:
  static PointerType *get(Type *baseType);

public:
  Type *getBaseType() const { return baseType; }
}; // class PointerType

//! Function type
class FunctionType : public Type {
private:
  Type *returnType;
  std::vector<Type *> paramTypes;

protected:
  FunctionType(Type *returnType, const std::vector<Type *> &paramTypes = {})
      : Type(kFunction), returnType(returnType), paramTypes(paramTypes) {}

public:
  static FunctionType *get(Type *returnType,
                           const std::vector<Type *> &paramTypes = {});

public:
  Type *getReturnType() const { return returnType; }
  auto getParamTypes() const { return make_range(paramTypes); }
  int getNumParams() const { return paramTypes.size(); }
}; // class FunctionType

/*!
 * @}
 */

/*!
 * \defgroup ir IR
 *
 * The SysY IR is an instruction level language. The IR is orgnized
 * as a four-level tree structure, as shown below
 *
 * \dotfile ir-4level.dot IR Structure
 *
 * - `Module` corresponds to the top level "CompUnit" syntax structure
 * - `GlobalValue` corresponds to the "Decl" syntax structure
 * - `Function` corresponds to the "FuncDef" syntax structure
 * - `BasicBlock` is a sequence of instructions without branching. A `Function`
 *   made up by one or more `BasicBlock`s.
 * - `Instruction` represents a primitive operation on values, e.g., add or sub.
 *
 * The fundamental data concept in SysY IR is `Value`. A `Value` is like
 * a register and is used by `Instruction`s as input/output operand. Each value
 * has an associated `Type` indicating the data type held by the value.
 *
 * Most `Instruction`s have a three-address signature, i.e., there are at most 2
 * input values and at most 1 output value.
 *
 * The SysY IR adots a Static-Single-Assignment (SSA) design. That is, `Value`
 * is defined (as the output operand ) by some instruction, and used (as the
 * input operand) by other instructions. While a value can be used by multiple
 * instructions, the `definition` occurs only once. As a result, there is a
 * one-to-one relation between a value and the instruction defining it. In other
 * words, any instruction defines a value can be viewed as the defined value
 * itself. So `Instruction` is also a `Value` in SysY IR. See `Value` for the
 * type hierachy.
 *
 * @{
 */

class User;
class Value;

//! `Use` represents the relation between a `Value` and its `User`
class Use {
private:
  //! the position of value in the user's operands, i.e.,
  //! user->getOperands[index] == value
  int index;
  User *user;
  Value *value;

public:
  Use() = default;
  Use(int index, User *user, Value *value)
      : index(index), user(user), value(value) {}

public:
  int getIndex() const { return index; }
  User *getUser() const { return user; }
  Value *getValue() const { return value; }
  void setValue(Value *value) { value = value; }
}; // class Use

//! The base class of all value types
class Value {
protected:
  Type *type;
  std::string name;
  std::list<Use *> uses;

protected:
  Value(Type *type, const std::string &name = "")
      : type(type), name(name), uses() {}
  virtual ~Value() {}

public:
  Type *getType() const { return type; }
  bool isInt() const { return type->isInt(); }
  bool isFloat() const { return type->isFloat(); }
  bool isPointer() const { return type->isPointer(); }
  const std::list<Use *> &getUses() { return uses; }
  void addUse(Use *use) { uses.push_back(use); }
  void replaceAllUsesWith(Value *value);
  void removeUse(Use *use) { uses.remove(use); }
}; // class Value

/*!
 * Static constants known at compile time.
 *
 * `ConstantValue`s are not defined by instructions, and do not use any other
 * `Value`s. It's type is either `int` or `float`.
 */
class ConstantValue : public Value {
protected:
  union {
    int iScalar;
    float fScalar;
  };

protected:
  ConstantValue(int value, const std::string &name = "")
      : Value(Type::getIntType(), name), iScalar(value) {}
  ConstantValue(float value, const std::string &name = "")
      : Value(Type::getFloatType(), name), fScalar(value) {}

public:
  static ConstantValue *get(int value, const std::string &name = "");
  static ConstantValue *get(float value, const std::string &name = "");

public:
  int getInt() const {
    assert(isInt());
    return iScalar;
  }
  float getFloat() const {
    assert(isFloat());
    return fScalar;
  }
}; // class ConstantValue

class BasicBlock;
/*!
 * Arguments of `BasicBlock`s.
 *
 * SysY IR is an SSA language, however, it does not use PHI instructions as in
 * LLVM IR. `Value`s from different predecessor blocks are passed explicitly as
 * block arguments. This is also the approach used by MLIR.
 * NOTE that `Function` does not own `Argument`s, function arguments are
 * implemented as its entry block's arguments.
 */

class Argument : public Value {
protected:
  BasicBlock *block;
  int index;

public:
  Argument(Type *type, BasicBlock *block, int index,
           const std::string &name = "")
      : Value(type, name), block(block), index(index) {}
};

class Instruction;
class Function;
/*!
 * The container for `Instruction` sequence.
 *
 * `BasicBlock` maintains a list of `Instruction`s, with the last one being
 * a terminator (branch or return). Besides, `BasicBlock` stores its arguments
 * and records its predecessor and successor `BasicBlock`s.
 */
class BasicBlock : public Value {
  friend class Function;

public:
  using inst_list = std::list<std::unique_ptr<Instruction>>;
  using iterator = inst_list::iterator;
  using arg_list = std::vector<Argument>;
  using block_list = std::vector<BasicBlock *>;

protected:
  Function *parent;
  inst_list instructions;
  arg_list arguments;
  block_list successors;
  block_list predecessors;

protected:
  explicit BasicBlock(Function *parent, const std::string &name = "")
      : Value(Type::getLabelType(), name), parent(parent), instructions(),
        arguments(), successors(), predecessors() {}

public:
  int getNumInstructions() const { return instructions.size(); }
  int getNumArguments() const { return arguments.size(); }
  int getNumPredecessors() const { return predecessors.size(); }
  int getNumSuccessors() const { return successors.size(); }
  Function *getParent() const { return parent; }
  inst_list &getInstructions() { return instructions; }
  arg_list &getArguments() { return arguments; }
  block_list &getPredecessors() { return predecessors; }
  block_list &getSuccessors() { return successors; }
  iterator begin() { return instructions.begin(); }
  iterator end() { return instructions.end(); }
  iterator terminator() { return std::prev(end()); }
  Argument *createArgument(Type *type, const std::string &name = "") {
    arguments.emplace_back(type, this, arguments.size(), name);
    return &arguments.back();
  };
}; // class BasicBlock

//! User is the abstract base type of `Value` types which use other `Value` as
//! operands. Currently, there are two kinds of `User`s, `Instruction` and
//! `GlobalValue`.
class User : public Value {
protected:
  std::vector<Use> operands;

protected:
  User(Type *type, const std::string &name = "")
      : Value(type, name), operands() {}

public:
  struct operand_iterator : std::vector<Use>::iterator {
    using Base = std::vector<Use>::iterator;
    using Base::Base;
    using value_type = Value *;
    value_type operator->() { return operator*().getValue(); }
  };

public:
  int getNumOperands() const { return operands.size(); }
  auto operand_begin() const { return operands.begin(); }
  auto operand_end() const { return operands.end(); }
  auto getOperands() const {
    return make_range(operand_begin(), operand_end());
  }
  Value *getOperand(int index) const { return operands[index].getValue(); }
  void addOperand(Value *value) {
    operands.emplace_back(operands.size(), this, value);
    value->addUse(&operands.back());
  }
  template <typename ContainerT> void addOperands(const ContainerT &operands) {
    for (auto value : operands)
      addOperand(value);
  }
  void replaceOperand(int index, Value *value);
  void setOperand(int index, Value *value);
  const std::string &getName() const { return name; }
}; // class User

/*!
 * Base of all concrete instruction types.
 */
class Instruction : public User {
public:
  enum Kind : uint64_t {
    kInvalid = 0x0UL,
    // Binary
    kAdd = 0x1UL << 0,
    kSub = 0x1UL << 1,
    kMul = 0x1UL << 2,
    kDiv = 0x1UL << 3,
    kRem = 0x1UL << 4,
    kICmpEQ = 0x1UL << 5,
    kICmpNE = 0x1UL << 6,
    kICmpLT = 0x1UL << 7,
    kICmpGT = 0x1UL << 8,
    kICmpLE = 0x1UL << 9,
    kICmpGE = 0x1UL << 10,
    kFAdd = 0x1UL << 14,
    kFSub = 0x1UL << 15,
    kFMul = 0x1UL << 16,
    kFDiv = 0x1UL << 17,
    kFRem = 0x1UL << 18,
    kFCmpEQ = 0x1UL << 19,
    kFCmpNE = 0x1UL << 20,
    kFCmpLT = 0x1UL << 21,
    kFCmpGT = 0x1UL << 22,
    kFCmpLE = 0x1UL << 23,
    kFCmpGE = 0x1UL << 24,
    // Unary
    kNeg = 0x1UL << 25,
    kNot = 0x1UL << 26,
    kFNeg = 0x1UL << 26,
    kFtoI = 0x1UL << 28,
    kIToF = 0x1UL << 29,
    // call
    kCall = 0x1UL << 30,
    // terminator
    kCondBr = 0x1UL << 31,
    kBr = 0x1UL << 32,
    kReturn = 0x1UL << 33,
    // mem op
    kAlloca = 0x1UL << 34,
    kLoad = 0x1UL << 35,
    kStore = 0x1UL << 36,
    // constant
    // kConstant = 0x1UL << 37,
  };

protected:
  Kind kind;
  BasicBlock *parent;

protected:
  Instruction(Kind kind, Type *type, BasicBlock *parent = nullptr,
              const std::string &name = "")
      : User(type, name), kind(kind), parent(parent) {}

public:
  Kind getKind() const { return kind; }
  BasicBlock *getParent() const { return parent; }
  void setParent(BasicBlock *bb) { parent = bb; }

  bool isBinary() const {
    static constexpr uint64_t BinaryOpMask =
        (kAdd | kSub | kMul | kDiv | kRem) |
        (kICmpEQ | kICmpNE | kICmpLT | kICmpGT | kICmpLE | kICmpGE) |
        (kFAdd | kFSub | kFMul | kFDiv | kFRem) |
        (kFCmpEQ | kFCmpNE | kFCmpLT | kFCmpGT | kFCmpLE | kFCmpGE);
    return kind & BinaryOpMask;
  }
  bool isUnary() const {
    static constexpr uint64_t UnaryOpMask = kNeg | kNot | kFNeg | kFtoI | kIToF;
    return kind & UnaryOpMask;
  }
  bool isMemory() const {
    static constexpr uint64_t MemoryOpMask = kAlloca | kLoad | kStore;
    return kind & MemoryOpMask;
  }
  bool isTerminator() const {
    static constexpr uint64_t TerminatorOpMask = kCondBr | kBr | kReturn;
    return kind & TerminatorOpMask;
  }
  bool isCmp() const {
    static constexpr uint64_t CmpOpMask =
        (kICmpEQ | kICmpNE | kICmpLT | kICmpGT | kICmpLE | kICmpGE) |
        (kFCmpEQ | kFCmpNE | kFCmpLT | kFCmpGT | kFCmpLE | kFCmpGE);
    return kind & CmpOpMask;
  }
  bool isBranch() const {
    static constexpr uint64_t BranchOpMask = kBr | kCondBr;
    return kind & BranchOpMask;
  }
  bool isCommutative() const {
    static constexpr uint64_t CommutativeOpMask =
        kAdd | kMul | kICmpEQ | kICmpNE | kFAdd | kFMul | kFCmpEQ | kFCmpNE;
    return kind & CommutativeOpMask;
  }
  bool isUnconditional() const { return kind == kBr; }
  bool isConditional() const { return kind == kCondBr; }
}; // class Instruction

class Function;
//! Function call.
class CallInst : public Instruction {
  friend class IRBuilder;

protected:
  CallInst(Function *callee, const std::vector<Value *> args = {},
           BasicBlock *parent = nullptr, const std::string &name = "");

public:
  Function *getCallee();
  auto getArguments() {
    return make_range(std::next(operand_begin()), operand_end());
  }
}; // class CallInst

//! Unary instruction, includes '!', '-' and type conversion.
class UnaryInst : public Instruction {
  friend class IRBuilder;

protected:
  UnaryInst(Kind kind, Type *type, Value *operand, BasicBlock *parent = nullptr,
            const std::string &name = "")
      : Instruction(kind, type, parent, name) {
    addOperand(operand);
  }

public:
  Value *getOperand() const { return User::getOperand(0); }
}; // class UnaryInst

//! Binary instruction, e.g., arithmatic, relation, logic, etc.
class BinaryInst : public Instruction {
  friend class IRBuilder;

protected:
  BinaryInst(Kind kind, Type *type, Value *lhs, Value *rhs, BasicBlock *parent,
             const std::string &name = "")
      : Instruction(kind, type, parent, name) {
    addOperand(lhs);
    addOperand(rhs);
  }

public:
  Value *getLhs() const { return getOperand(0); }
  Value *getRhs() const { return getOperand(1); }
}; // class BinaryInst

//! The return statement
class ReturnInst : public Instruction {
  friend class IRBuilder;

protected:
  ReturnInst(Value *value = nullptr, BasicBlock *parent = nullptr)
      : Instruction(kReturn, Type::getVoidType(), parent, "") {
    if (value)
      addOperand(value);
  }

public:
  bool hasReturnValue() const { return not operands.empty(); }
  Value *getReturnValue() const {
    return hasReturnValue() ? getOperand(0) : nullptr;
  }
}; // class ReturnInst

//! Unconditional branch
class UncondBrInst : public Instruction {
  friend class IRBuilder;

protected:
  UncondBrInst(BasicBlock *block, std::vector<Value *> args,
               BasicBlock *parent = nullptr)
      : Instruction(kCondBr, Type::getVoidType(), parent, "") {
    assert(block->getNumArguments() == args.size());
    addOperand(block);
    addOperands(args);
  }

public:
  BasicBlock *getBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(0));
  }
  auto getArguments() const {
    return make_range(std::next(operands.begin()), operands.end());
  }
}; // class UncondBrInst

//! Conditional branch
class CondBrInst : public Instruction {
  friend class IRBuilder;

protected:
  CondBrInst(Value *condition, BasicBlock *thenBlock, BasicBlock *elseBlock,
             const std::vector<Value *> &thenArgs,
             const std::vector<Value *> &elseArgs, BasicBlock *parent = nullptr)
      : Instruction(kCondBr, Type::getVoidType(), parent, "") {
    assert(thenBlock->getNumArguments() == thenArgs.size() and
           elseBlock->getNumArguments() == elseArgs.size());
    addOperand(condition);
    addOperand(thenBlock);
    addOperand(elseBlock);
    addOperands(thenArgs);
    addOperands(elseArgs);
  }

public:
  BasicBlock *getThenBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(0));
  }
  BasicBlock *getElseBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(1));
  }
  auto getThenArguments() const {
    auto begin = operands.begin() + 2;
    auto end = begin + getThenBlock()->getNumArguments();
    return make_range(begin, end);
  }
  auto getElseArguments() const {
    auto begin = operands.begin() + 2 + getThenBlock()->getNumArguments();
    auto end = operands.end();
    return make_range(begin, end);
  }
}; // class CondBrInst

//! Allocate memory for stack variables, used for non-global variable declartion
class AllocaInst : public Instruction {
  friend class IRBuilder;

protected:
  AllocaInst(Type *type, const std::vector<Value *> &dims = {},
             BasicBlock *parent = nullptr, const std::string &name = "")
      : Instruction(kAlloca, type, parent, name) {
    addOperands(dims);
  }

public:
  int getNumDims() const { return getNumOperands(); }
  auto getDims() const { return getOperands(); }
  Value *getDim(int index) { return getOperand(index); }
}; // class AllocaInst

//! Load a value from memory address specified by a pointer value
class LoadInst : public Instruction {
  friend class IRBuilder;

protected:
  LoadInst(Value *pointer, const std::vector<Value *> &indices = {},
           BasicBlock *parent = nullptr, const std::string &name = "")
      : Instruction(kLoad, pointer->getType()->as<PointerType>()->getBaseType(),
                    parent, name) {
    addOperands(indices);
  }

public:
  int getNumIndices() const { return getNumOperands() - 1; }
  Value *getPointer() const { return getOperand(0); }
  auto getIndices() const {
    return make_range(std::next(operand_begin()), operand_end());
  }
  Value *getIndex(int index) const { return getOperand(index + 1); }
}; // class LoadInst

//! Store a value to memory address specified by a pointer value
class StoreInst : public Instruction {
  friend class IRBuilder;

protected:
  StoreInst(Value *value, Value *pointer,
            const std::vector<Value *> &indices = {},
            BasicBlock *parent = nullptr, const std::string &name = "")
      : Instruction(kStore, Type::getVoidType(), parent, name) {
    addOperand(value);
    addOperand(pointer);
    addOperands(indices);
  }

public:
  int getNumIndices() const { return getNumOperands() - 2; }
  Value *getValue() const { return getOperand(0); }
  Value *getPointer() const { return getOperand(1); }
  auto getIndices() const {
    return make_range(operand_begin() + 2, operand_end());
  }
  Value *getIndex(int index) const { return getOperand(index + 2); }
}; // class StoreInst

class Module;
//! Function definition
class Function : public Value {
  friend class Module;

protected:
  Function(Module *parent, Type *type, const std::string &name)
      : Value(type, name), parent(parent), blocks() {
    blocks.emplace_back(new BasicBlock(this, "entry"));
  }

public:
  using block_list = std::list<std::unique_ptr<BasicBlock>>;

protected:
  Module *parent;
  block_list blocks;

public:
  Type *getReturnType() const {
    return getType()->as<FunctionType>()->getReturnType();
  }
  auto getParamTypes() const {
    return getType()->as<FunctionType>()->getParamTypes();
  }
  auto getBasicBlocks() { return make_range(blocks); }
  BasicBlock *getEntryBlock() { return blocks.front().get(); }
  BasicBlock *addBasicBlock(const std::string &name = "") {
    blocks.emplace_back(new BasicBlock(this, name));
    return blocks.back().get();
  }
  void removeBasicBlock(BasicBlock *block) {
    blocks.remove_if([&](std::unique_ptr<BasicBlock> &b) -> bool {
      return block == b.get();
    });
  }
}; // class Function

//! Global value declared at file scope
class GlobalValue : public User {
  friend class Module;

protected:
  Module *parent;
  bool hasInit;

protected:
  GlobalValue(Module *parent, Type *type, const std::string &name,
              const std::vector<Value *> &dims = {}, Value *init = nullptr)
      : User(type, name), parent(parent), hasInit(init) {
    assert(type->isPointer());
    addOperands(dims);
    if (init)
      addOperand(init);
  }

public:
  Value *init() const { return hasInit ? operands.back().getValue() : nullptr; }
  int getNumDims() const { return getNumOperands() - (hasInit ? 1 : 0); }
  Value *getDim(int index) { return getOperand(index); }
}; // class GlobalValue

//! IR unit for representing a SysY compile unit
class Module {
protected:
  std::map<std::string, std::unique_ptr<Function>> functions;
  std::map<std::string, std::unique_ptr<GlobalValue>> globals;

public:
  Module() = default;

public:
  Function *createFunction(const std::string &name, Type *type) {
    auto result = functions.try_emplace(name, new Function(this, type, name));
    if (not result.second)
      return nullptr;
    return result.first->second.get();
  };
  GlobalValue *createGlobalValue(const std::string &name, Type *type,
                                 const std::vector<Value *> &dims = {}) {
    auto result =
        globals.try_emplace(name, new GlobalValue(this, type, name, dims));
    if (not result.second)
      return nullptr;
    return result.first->second.get();
  }
  Function *getFunction(const std::string &name) const {
    auto result = functions.find(name);
    if (result == functions.end())
      return nullptr;
    return result->second.get();
  }
  GlobalValue *getGlobalValue(const std::string &name) const {
    auto result = globals.find(name);
    if (result == globals.end())
      return nullptr;
    return result->second.get();
  }
}; // class Module

/*!
 * @}
 */
} // namespace sysy