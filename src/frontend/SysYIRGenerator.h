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

extern std::map<std::string, std::vector<Value *>> usedarrays;

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

  virtual std::any visitScalarInitValue(SysYParser::ScalarInitValueContext *ctx) override;

  virtual std::any visitArrayInitValue(SysYParser::ArrayInitValueContext *ctx) override;
  
  virtual std::any visitFunc(SysYParser::FuncContext *ctx) override;

  virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;

  virtual std::any
  visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override {
    return visitChildren(ctx);
  }

  // virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

  std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx, bool parallelized = false) {
    for (auto item : ctx->blockItem()) {
      // if (1) {
      //   std::cout << (item->decl()? "decl": "no decl") << (item->stmt()? "stmt": "no stmt") << std::endl;
      // }
      visitBlockItem(item);
    }
    if(parallelized) {
      auto *curBb = builder.getBasicBlock();
      curBb->setInPragma(true);
    }
    return builder.getBasicBlock();
  }

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

  virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override;

  virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override;

  virtual std::any visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override;

  virtual std::any visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override;

  virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationExp(SysYParser::RelationExpContext *ctx) override;

  virtual std::any visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override;

  virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override;

  virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override;

  virtual std::any visitAndExp(SysYParser::AndExpContext *ctx) override;

  virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;

  virtual std::any visitPragmaStmt(SysYParser::PragmaStmtContext *ctx) override;

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

private:
  std::map<std::string, size_t> globalBlockSegs = {{"block", 0}, {"br", 0}};
public:
  std::string emitBlockName(const std::string &hint = "", const int allocatedBlockID = -1) {
    if (hint == "") {
      globalBlockSegs["block"]++;
      return "block" + std::to_string(globalBlockSegs["block"]);
    }
    if (allocatedBlockID == -1) {
      globalBlockSegs["br"]++;
      return hint + std::to_string(globalBlockSegs["br"]);
    }
    else {
      return hint + std::to_string(allocatedBlockID);
    }
  }
  int AllocateNamedBlockID(const std::string hint = "") {
    if (hint == "") {
      return ++globalBlockSegs["block"];
    }
    else return ++globalBlockSegs["br"];
  }
}; // class SysYIRGenerator

} // namespace sysy