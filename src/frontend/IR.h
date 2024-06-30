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
#include <iostream>
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
    kInt = 0,
    kFloat = 1,
    kVoid = 2,
    kLabel = 3,
    kPointer = 4,
    kFunction = 5,
  };
  Kind kind;

protected:
  Type(Kind kind) : kind(kind) {}
  virtual ~Type() = default;

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
  bool isIntOrFloat() const { return kind == kInt or kind == kFloat; }
  int getSize() const;
  template <typename T>
  std::enable_if_t<std::is_base_of_v<Type, T>, T *> as() const {
    return dynamic_cast<T *>(const_cast<Type *>(this));
  }
  void print(std::ostream &os) const;
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

template <typename T>
inline std::enable_if_t<std::is_base_of_v<Value, T>, bool>
isa(const Value *value) {
  return T::classof(value);
}

template <typename T>
inline std::enable_if_t<std::is_base_of_v<Value, T>, T *>
dynamicCast(Value *value) {
  return isa<T>(value) ? static_cast<T *>(value) : nullptr;
}

template <typename T>
inline std::enable_if_t<std::is_base_of_v<Value, T>, const T *>
dynamicCast(const Value *value) {
  return isa<T>(value) ? static_cast<const T *>(value) : nullptr;
}

//! The base class of all value types
class Value {
public:
enum Kind : uint64_t {
    kInvalid,
    // Binary
    kAdd,
    kSub,
    kMul,
    kDiv,
    kRem,
    kICmpEQ,
    kICmpNE,
    kICmpLT,
    kICmpGT,
    kICmpLE,
    kICmpGE,
    kFAdd,
    kFSub,
    kFMul,
    kFDiv,
    kFRem,
    kFCmpEQ,
    kFCmpNE,
    kFCmpLT,
    kFCmpGT,
    kFCmpLE,
    kFCmpGE,
    kOr,
    kAnd,
    kFOr,
    kFAnd,
    kSLL,
    // Unary
    kNeg,
    kNot,
    kFNeg,
    kFtoI,
    kItoF,
    // call
    kCall,
    // terminator
    kCondBr,
    kBr,
    kReturn,
    // mem op
    kAlloca,
    kLoad,
    kStore,
    kFirstInst = kAdd,
    kLastInst = kStore,
    // constant
    // kConstant = 37,
    kArgument,
    kBasicBlock,
    kFunction,
    kConstant,
    kGlobal,
  };
  
protected:
  Kind kind;
  Type *type;
  std::string name;
  std::list<Use *> uses;

public:
  Value(Kind kind, Type *type, const std::string &name = "")
      : kind(kind), type(type), name(name), uses() {}

public:
  virtual ~Value() = default;

public:
  Kind getKind() const { return kind; }
  void setKind(Kind k) {this->kind = k; }
  static bool classof(const Value *) { return true; }

public:
  Type *getType() const { return type; }
  const std::string &getName() const {return name; }
  void setName(const std::string &n) {name = n; }
  bool hasName() const { return not name.empty(); }
  bool isInt() const { return type->isInt(); }
  bool isFloat() const { return type->isFloat(); }
  bool isPointer() const { return type->isPointer(); }
  const std::list<Use *> &getUses() { return uses; }
  void addUse(Use *use) { uses.push_back(use); }
  void replaceAllUsesWith(Value *value);
  void removeUse(Use *use) { uses.remove(use); }
  bool isConstant() const;

public:
  virtual void print(std::ostream &os) const {};
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

public:
  ConstantValue(int value, const std::string &name = "")
      : Value(kConstant, Type::getIntType(), name), iScalar(value) {}
  ConstantValue(float value, const std::string &name = "")
      : Value(kConstant, Type::getFloatType(), name), fScalar(value) {}

public:
  static ConstantValue *get(int value, const std::string &name = "");
  static ConstantValue *get(float value, const std::string &name = "");

public:
  static bool classof(const Value *value) {
    return value->getKind() == kConstant;
  }

public:
  int getInt() const {
    assert(isInt());
    return iScalar;
  }
  float getFloat() const {
    assert(isFloat());
    return fScalar;
  }
  void setInt(int v) {
    iScalar = v;
  }
  void setFloat(float v) {
    fScalar = v;
  }

public:
  void print(std::ostream &os) const;
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
  Argument(Type *type, BasicBlock *block, int index, const std::string &name = "");
  // : Value(kArgument, type, name), block(block), index(index) {
  //   if (not hasName()) {
  //     setName(std::to_string(block->getParent()->allocateVariableID()));
  //   }
  // }

public:
  static bool classof(const Value *value) {
    return value->getKind() == kConstant;
  }

public:
  BasicBlock *getParent() const { return block; }
  int getIndex() const { return index; }

public:
  void print(std::ostream &os) const;
};

class Instruction;
class Function;

class RVInst {
public:
  std::string space = "  ";
  std::string endl = "\n";
  std::string op;
  std::vector<std::string> fields;
  bool valid;
  RVInst() = default;
  RVInst(std::string op):
    op(op) {};
  RVInst(std::string op, std::string field1):
    op(op) {
      this->fields.push_back(field1);
    };
  RVInst(std::string op, std::string field1, std::string field2):
    op(op) {
      this->fields.push_back(field1);
      this->fields.push_back(field2);
    };
  RVInst(std::string op, std::string field1, std::string field2, std::string field3):
    op(op) {
      this->fields.push_back(field1);
      this->fields.push_back(field2);
      this->fields.push_back(field3);
    };
  RVInst(std::string op, std::string field1, std::string field2, std::string field3, std::string field4):
    op(op) {
      this->fields.push_back(field1);
      this->fields.push_back(field2);
      this->fields.push_back(field3);
      this->fields.push_back(field4);
    };
  void print(std::ostream &os) {
    // if (!this->valid) { return; }
    os << this->space << this->op << " ";
    // for (auto &it: this->fields) {
    //   os << it;
    //   if (it != this->fields.back()) {
    //     os << ", ";
    //   }
    // }
    for (int i = 0; i < this->fields.size(); i++){
      os << this->fields[i];
      if (i != this->fields.size()-1){
        os << ", ";
      }
    }
    os << endl;
  }
};

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
  using arg_list = std::vector<std::unique_ptr<Argument>>;
  using block_list = std::vector<BasicBlock *>;
  bool upleveled = false;

public:
  enum BBKind {
    kNormal,
    kEntry,
    kWhileHeader,
    kWhileBody,
    kWhileEnd
  };

  std::vector<RVInst> CoInst;
  std::string bbLabel;
protected:
  Function *parent;
  inst_list instructions;
  arg_list arguments;
  block_list successors;
  block_list predecessors;
  bool inPragma;
  BBKind kind;

protected:
  explicit BasicBlock(Function *parent, const std::string &name = "");
//   : Value(kBasicBlock, Type::getLabelType(), name), parent(parent),
//       instructions(), arguments(), successors(), predecessors() {
//   if (not hasName())
//     setName("bb" + std::to_string(getParent()->allocateblockID()));
// }

public:
  int getNumInstructions() const { return instructions.size(); }
  int getNumArguments() const { return arguments.size(); }
  int getNumPredecessors() const { return predecessors.size(); }
  int getNumSuccessors() const { return successors.size(); }
  Function *getParent() const { return parent; }
  inst_list &getInstructions() { return instructions; }
  auto getArguments() { return make_range(arguments); }
  block_list &getPredecessors() { return predecessors; }
  block_list &getSuccessors() { return successors; }
  iterator begin() { return instructions.begin(); }
  iterator end() { return instructions.end(); }
  iterator terminator() { return std::prev(end()); }
  Argument *createArgument(Type *type, const std::string &name = "") {
    auto arg = new Argument(type, this, arguments.size(), name);
    assert(arg);
    arguments.emplace_back(arg);
    return arguments.back().get();
  };
  bool isInPragma() { return this->inPragma; }
  void setInPragma(bool inPragma) { this->inPragma = inPragma; }
  void setKind(BBKind kind) { this->kind = kind ;}
  BBKind getKind() { return kind; }

public:
  void print(std::ostream &os);
}; // class BasicBlock

//! User is the abstract base type of `Value` types which use other `Value` as
//! operands. Currently, there are two kinds of `User`s, `Instruction` and
//! `GlobalValue`.
class User : public Value {
protected:
  std::vector<Use> operands;

protected:
  User(Kind kind, Type *type, const std::string &name = "")
      : Value(kind, type, name), operands() {}

public:
  struct operand_iterator : std::vector<Use>::iterator {
    using Base = std::vector<Use>::iterator;
    using Base::Base;
    using value_type = Value *;
    value_type operator->() { return Base::operator*().getValue(); }
    value_type operator*() { return Base::operator*().getValue(); }
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
  int inst_index;
  int last_used;
protected:
  Kind kind;
  BasicBlock *parent;

protected:
  Instruction(Kind kind, Type *type, BasicBlock *parent = nullptr,
              const std::string &name = "");
//               :User(kind, type, name), kind(kind), parent(parent) {
//   if (not type->isVoid() and not hasName())
//     setName(std::to_string(getFunction()->allocateVariableID()));
// }

public:
  static bool classof(const Value *value) {
    return value->getKind() >= kFirstInst and value->getKind() <= kLastInst;
  }

public:
  Kind getKind() const { return kind; }
  BasicBlock *getParent() const { return parent; }
  Function *getFunction() const { return parent->getParent(); }
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
    static constexpr uint64_t UnaryOpMask = kNeg | kNot | kFNeg | kFtoI | kItoF;
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
  static bool classof(const Value *value) { return value->getKind() == kCall; }

public:
  Function *getCallee() const;
  auto getArguments() const {
    return make_range(std::next(operand_begin()), operand_end());
  }

public:
  void print(std::ostream &os) const;
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
  static bool classof(const Value *value) {
    return Instruction::classof(value) and
           static_cast<const Instruction *>(value)->isUnary();
  }

public:
  Value *getOperand() const { return User::getOperand(0); }

public:
  void print(std::ostream &os) const;
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
  static bool classof(const Value *value) {
    return Instruction::classof(value) and
           static_cast<const Instruction *>(value)->isBinary();
  }

public:
  Value *getLhs() const { return getOperand(0); }
  Value *getRhs() const { return getOperand(1); }

public:
  void print(std::ostream &os) const;
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
  static bool classof(const Value *value) {
    return value->getKind() == kReturn;
  }

public:
  bool hasReturnValue() const { return not operands.empty(); }
  Value *getReturnValue() const {
    return hasReturnValue() ? getOperand(0) : nullptr;
  }

public:
  void print(std::ostream &os) const;
}; // class ReturnInst

//! Unconditional branch
class UncondBrInst : public Instruction {
  friend class IRBuilder;

protected:
  UncondBrInst(BasicBlock *block, std::vector<Value *> args,
               BasicBlock *parent = nullptr)
      : Instruction(kBr, Type::getVoidType(), parent, "") {
    assert(block->getNumArguments() == args.size());
    addOperand(block);
    addOperands(args);
  }

public:
  static bool classof(const Value *value) { return value->getKind() == kBr; }

public:
  BasicBlock *getBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(0));
  }
  auto getArguments() const {
    return make_range(std::next(operands.begin()), operands.end());
  }

public:
  void print(std::ostream &os) const;
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
  static bool classof(const Value *value) {
    return value->getKind() == kCondBr;
  }

public:
  Value *getCondition() const {return getOperand(0); }
  BasicBlock *getThenBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(1));
  }
  BasicBlock *getElseBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(2));
  }
  auto getThenArguments() const {
    auto begin = std::next(operand_begin(), 3);
    auto end = std::next(begin, getThenBlock()->getNumArguments());
    return make_range(begin, end);
  }
  auto getElseArguments() const {
    auto begin =
        std::next(operand_begin(), 3 + getThenBlock()->getNumArguments());
    auto end = operand_end();
    return make_range(begin, end);
  }

public:
  void print(std::ostream &os) const;
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
  static bool classof(const Value *value) {
    return value->getKind() == kAlloca;
  }

public:
  int getNumDims() const { return getNumOperands(); }
  auto getDims() const { return getOperands(); }
  Value *getDim(int index) const { return getOperand(index); }

public:
  void print(std::ostream &os) const;
}; // class AllocaInst

//! Load a value from memory address specified by a pointer value
class LoadInst : public Instruction {
  friend class IRBuilder;

protected:
  LoadInst(Value *pointer, const std::vector<Value *> &indices = {},
           BasicBlock *parent = nullptr, const std::string &name = "")
      : Instruction(kLoad, pointer->getType()->as<PointerType>()->getBaseType(),
                    parent, name) {
    addOperand(pointer);
    addOperands(indices);
  }

public:
  static bool classof(const Value *value) { return value->getKind() == kLoad; }


public:
  int getNumIndices() const { return getNumOperands() - 1; }
  Value *getPointer() const { return getOperand(0); }
  auto getIndices() const {
    return make_range(std::next(operand_begin()), operand_end());
  }
  Value *getIndex(int index) const { return getOperand(index + 1); }

public:
  void print(std::ostream &os) const;
}; // class LoadInst

//! Store a value to memory address specified by a pointer value
class StoreInst : public Instruction {
  friend class IRBuilder;

public:
  bool inarray = false;
  std::vector<Value *> indices;
protected:
  StoreInst(Value *value, Value *pointer,
            const std::vector<Value *> &indices = {},
            BasicBlock *parent = nullptr, const std::string &name = "")
      : Instruction(kStore, Type::getVoidType(), parent, name) {
    addOperand(value);
    addOperand(pointer);
    addOperands(indices);
    for (auto &it: indices) {
      this->indices.push_back(it);
    }
  }

public:
  static bool classof(const Value *value) { return value->getKind() == kStore; }

public:
  int getNumIndices() const { return getNumOperands() - 2; }
  Value *getValue() const { return getOperand(0); }
  Value *getPointer() const { return getOperand(1); }
  auto getIndices() const {
    return make_range(operand_begin() + 2, operand_end());
  }
  auto getOffset() const {
    return this->indices[0];
  }
  Value *getIndex(int index) const { return getOperand(index + 2); }

public:
  void print(std::ostream &os) const;
}; // class StoreInst

class Module;
//! Function definition
class Function : public Value {
  friend class Module;

public:
  Function(Module *parent, Type *type, const std::string &name)
      : Value(kFunction, type, name), parent(parent), blocks() {
    blocks.emplace_back(new BasicBlock(this, "entry"));
    this->variableID = 0;
  }

public:
  static bool classof(const Value *value) {
    return value->getKind() == kFunction;
  }
  std::vector<RVInst> MetaInst;
  std::vector<RVInst> PostInst;

public:
  using block_list = std::list<std::unique_ptr<BasicBlock>>;

protected:
  Module *parent;
  int variableID;
  int blockID;
  block_list blocks;

public:
  Type *getReturnType() const {
    return getType()->as<FunctionType>()->getReturnType();
  }
  auto getParamTypes() const {
    return getType()->as<FunctionType>()->getParamTypes();
  }
  auto getBasicBlocks() const { return make_range(blocks); }
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
  int allocateVariableID() { return variableID++; }
  int allocateblockID() { return blockID++; }

public:
  void print(std::ostream &os) const;
}; // class Function

//! Global value declared at file scope
class GlobalValue : public User {
  friend class Module;

protected:
  Module *parent;
  bool hasInit;
  bool isConst;
  std::vector<Value*> initVals;

protected:
  GlobalValue(Module *parent, Type *type, const std::string &name,
              const std::vector<Value *> &dims = {}, const std::vector<Value *> &init = {})
      : User(kGlobal, type, name), parent(parent), hasInit(init.empty() == false) {
    assert(type->isPointer());
    addOperands(dims);
    if (init.empty() == false)
      addOperands(init);
      addInitVals(init);
  }

public:
  static bool classof(const Value *value) {
    return value->getKind() == kGlobal;
  }

public:
  Value *init() const { return hasInit ? operands.back().getValue() : nullptr; }
  int getNumDims() const { return getNumOperands() - (hasInit ? this->initVals.size() : 0); }
  Value *getDim(int index) const { return getOperand(index); }
  void addInitVals(const std::vector<Value *> &init) {for (auto &it: init) this->initVals.push_back(it);}
  int getNumInitVals() const { return this->initVals.size(); }
  Value *getInitVals(int index) const {return this->initVals[index]; }
  void print(std::ostream &os) const;
}; // class GlobalValue

//! IR unit for representing a SysY compile unit
class Module {
protected:
  std::vector<std::unique_ptr<Value>> children;
  std::map<std::string, Function*> functions;
  std::map<std::string, GlobalValue*> globals;

public:
  Module() = default;
  std::string descriptionText;
  std::string globalDataText;
  std::string srcFile;
  int staticSize;

public:
  Function *createFunction(const std::string &name, Type *type) {
    if (functions.count(name)) {
      return nullptr;
    }
    auto func = new Function(this, type, name);
    assert(func);
    this->children.emplace_back(func);
    functions.emplace(name, func);
    return func;
  };
  GlobalValue *createGlobalValue(const std::string &name, Type *type,
                                 const std::vector<Value *> &dims = {}, const std::vector<Value *> &init = {}) {
    if (globals.count(name)) {
      return nullptr;
    }
    auto global = new GlobalValue(this, type, name, dims, init);
    assert(global);
    this->children.emplace_back(global);
    this->globals.emplace(name, global);
    return global;
  }
  Function *getFunction(const std::string &name) const {
    auto result = functions.find(name);
    if (result == functions.end())
      return nullptr;
    return result->second;
  }
  GlobalValue *getGlobalValue(const std::string &name) const {
    auto result = globals.find(name);
    if (result == globals.end())
      return nullptr;
    return result->second;
  }

public:
  void print(std::ostream &os) const;

public:
  std::map<std::string, Function*> *getFunctions() { return &functions; }
  std::map<std::string, GlobalValue*> *getGlobalValues() { return &globals; }
}; // class Module

/*!
 * @}
 */

/* for automatic print */
inline std::ostream &operator<<(std::ostream &os, const Type &type) {
  type.print(os);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Value &value) {
  value.print(os);
  return os;
}
} // namespace sysy