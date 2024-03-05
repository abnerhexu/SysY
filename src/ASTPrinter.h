#pragma once
#include <algorithm>
#include <iostream>
#include "SysYBaseVisitor.h"

class ASTPrinter : public SysYBaseVisitor {
public:
  std::any visitCompUnit(SysYParser::CompUnitContext *ctx) override;

  std::any visitDataType(SysYParser::DataTypeContext *ctx) override;

  std::any visitDecl(SysYParser::DeclContext *ctx) override;

  std::any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;

  std::any visitConstDef(SysYParser::ConstDefContext *ctx) override;

  std::any visitConstInitVal(SysYParser::ConstInitValContext *ctx) override;

  std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override;

  std::any visitVarDef(SysYParser::VarDefContext *ctx) override;

  std::any visitInitVal(SysYParser::InitValContext *ctx) override;

  std::any visitFuncDef(SysYParser::FuncDefContext *ctx) override;

  std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;

  std::any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override;

  std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override;

  std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

  std::any visitBlockItem(SysYParser::BlockItemContext *ctx) override;

  std::any visitStmt(SysYParser::StmtContext *ctx) override;

  std::any visitExpr(SysYParser::ExprContext *ctx) override;

  std::any visitCond(SysYParser::CondContext *ctx) override;

  std::any visitLhsVal(SysYParser::LhsValContext *ctx) override;

  std::any visitPrimaryExpr(SysYParser::PrimaryExprContext *ctx) override;

  std::any visitNumber(SysYParser::NumberContext *ctx) override;

  std::any visitString(SysYParser::StringContext *ctx) override;

  std::any visitUnaryExpr(SysYParser::UnaryExprContext *ctx) override;

  std::any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;

  std::any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;

  std::any visitMulExpr(SysYParser::MulExprContext *ctx) override;

  std::any visitAddExpr(SysYParser::AddExprContext *ctx) override;

  std::any visitRelExpr(SysYParser::RelExprContext *ctx) override;

  std::any visitEqExpr(SysYParser::EqExprContext *ctx) override;

  std::any visitLAndExpr(SysYParser::LAndExprContext *ctx) override;

  std::any visitLOrExpr(SysYParser::LOrExprContext *ctx) override;

  std::any visitConstExpr(SysYParser::ConstExprContext *ctx) override;
};

extern std::size_t indentLevel;

void printIndent(std::size_t level);