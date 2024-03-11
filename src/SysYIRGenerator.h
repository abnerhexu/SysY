#pragma once

#include <memory>
#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"

namespace sysy {

class SysYIRGenerator : public SysYBaseVisitor {
private:
  std::unique_ptr<Module> module;
  IRBuilder builder;

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

  virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override {
    return visitChildren(ctx);
  }

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

  virtual std::any
  visitAssignStmt(SysYParser::AssignStmtContext *ctx) override {
    return visitChildren(ctx);
  }

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

  virtual std::any
  visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitRelationExp(SysYParser::RelationExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any
  visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override {
    return visitChildren(ctx);
  }

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

  virtual std::any
  visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualExp(SysYParser::EqualExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCall(SysYParser::CallContext *ctx) override {
    return visitChildren(ctx);
  }

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

}; // class SysYIRGenerator

} // namespace sysy