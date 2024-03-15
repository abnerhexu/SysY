#pragma once

#include <memory>
#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include <forward_list>
#include <cassert>
#include <string>

namespace sysy {

class SymbolTable {
private:
  enum Kind {
    kModule,
    kFunction,
    kBlock,
  };

public:
  struct ModuleScope {
    SymbolTable &table;
    ModuleScope(SymbolTable &table) : table(table) { table.enter(kModule); }
    ~ModuleScope() { table.exit(); }
  };
  struct FunctionScope {
    SymbolTable &table;
    FunctionScope(SymbolTable &table) : table(table) { table.enter(kFunction); }
    ~FunctionScope() { table.exit(); }
  };

  struct BlockScope {
    SymbolTable &table;
    BlockScope(SymbolTable &table) : table(table) { table.enter(kBlock); }
    ~BlockScope() { table.exit(); }
  };

private:
  std::forward_list<std::pair<Kind, std::map<std::string, Value *>>> symbols;

public:
  SymbolTable() = default;

public:
  bool isModuleScope() const { return symbols.front().first == kModule; }
  bool isFunctionScope() const { return symbols.front().first == kFunction; }
  bool isBlockScope() const { return symbols.front().first == kBlock; }
  Value *lookup(const std::string &name) const {
    for (auto &scope : symbols) {
      auto iter = scope.second.find(name);
      if (iter != scope.second.end())
        return iter->second;
    }
    return nullptr;
  }
  auto insert(const std::string &name, Value *value) {
    assert(not symbols.empty());
    return symbols.front().second.emplace(name, value);
  }
  // void remove(const std::string &name) {
  //   for (auto &scope : symbols) {
  //     auto iter = scope.find(name);
  //     if (iter != scope.end()) {
  //       scope.erase(iter);
  //       return;
  //     }
  //   }
  // }
private:
  void enter(Kind kind) {
    symbols.emplace_front();
    symbols.front().first = kind;
  }
  void exit() { symbols.pop_front(); }
}; // class SymbolTable

class SysYIRGenerator : public SysYBaseVisitor {
private:
  std::unique_ptr<Module> module;
  IRBuilder builder;
  SymbolTable symbols;

public:
  SysYIRGenerator() = default;

public:
  virtual std::any visitModule(SysYParser::ModuleContext *ctx) override;

  virtual std::any visitDecl(SysYParser::DeclContext *ctx) override;

  virtual std::any visitBtype(SysYParser::BtypeContext *ctx) override;
  
  virtual std::any visitVarDef(SysYParser::VarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitScalarInitValue(SysYParser::ScalarInitValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitArrayInitValue(SysYParser::ArrayInitValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunc(SysYParser::FuncContext *ctx) override;

  // virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;

  virtual std::any
  visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitFuncFParam(SysYParser::FuncFParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

  virtual std::any visitBlockItem(SysYParser::BlockItemContext *ctx) override;

  virtual std::any visitStmt(SysYParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any
  // visitAssignStmt(SysYParser::AssignStmtContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override;

  virtual std::any visitExpStmt(SysYParser::ExpStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStmt(SysYParser::IfStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any
  // visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override {
  //   return visitChildren(ctx);
  // }

virtual std::any visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override;

  virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitRelationExp(SysYParser::RelationExpContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any
  // visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override;

  // virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override;

  // virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override;

  virtual std::any visitAndExp(SysYParser::AndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenExp(SysYParser::ParenExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringExp(SysYParser::StringExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrExp(SysYParser::OrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallExp(SysYParser::CallExpContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any
  // visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override;

  virtual std::any visitEqualExp(SysYParser::EqualExpContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any visitCall(SysYParser::CallContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitCall(SysYParser::CallContext *ctx) override;

  virtual std::any visitLValue(SysYParser::LValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(SysYParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitString(SysYParser::StringContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override {
    return visitChildren(ctx);
  }

private:
  std::any visitGlobalDecl(SysYParser::DeclContext *ctx);
  std::any visitLocalDecl(SysYParser::DeclContext *ctx);
  Type *getArithmeticResultType(Type *lhs, Type *rhs) {
    assert(lhs->isIntOrFloat() and rhs->isIntOrFloat());
    return lhs == rhs ? lhs : Type::getFloatType();
  }

}; // class SysYIRGenerator

} // namespace sysy