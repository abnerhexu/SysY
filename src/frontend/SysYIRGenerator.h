#pragma once

#include <memory>
#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include "SymbolTable.h"
#include <forward_list>
#include <cassert>
#include <string>

namespace sysy {

class SysYIRGenerator : public SysYBaseVisitor {
private:
  std::unique_ptr<Module> module;
  IRBuilder builder;
  SymbolTable symbols;

public:
  SysYIRGenerator() = default;

public:
  Module *get() const { return module.get(); }

public:
  virtual std::any visitModule(SysYParser::ModuleContext *ctx) override;

  virtual std::any visitDecl(SysYParser::DeclContext *ctx) override;

  virtual std::any visitBtype(SysYParser::BtypeContext *ctx) override;

  virtual std::any visitVarDef(SysYParser::VarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any visitInitValue(SysYParser::InitValueContext *ctx) override {
  //   return visitChildren(ctx);
  // }

  virtual std::any visitScalarInitValue(SysYParser::ScalarInitValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayInitValue(SysYParser::ArrayInitValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunc(SysYParser::FuncContext *ctx) override;

  virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;

  virtual std::any
  visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

  // virtual std::any visitBlockItem(SysYParser::BlockItemContext *ctx)
  // override;

  virtual std::any visitStmt(SysYParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override;

  virtual std::any visitExpStmt(SysYParser::ExpStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStmt(SysYParser::IfStmtContext *ctx) override;

  virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override;

  virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationExp(SysYParser::RelationExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override;

  virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override;

  virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override;

  virtual std::any visitAndExp(SysYParser::AndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;

  virtual std::any visitParenExp(SysYParser::ParenExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringExp(SysYParser::StringExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrExp(SysYParser::OrExpContext *ctx) override;

  virtual std::any visitCallExp(SysYParser::CallExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override;

  virtual std::any visitEqualExp(SysYParser::EqualExpContext *ctx) override;

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

  virtual std::any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override {
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