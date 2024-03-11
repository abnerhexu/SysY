#pragma once

#include "SysYBaseListener.h"
#include <ostream>

class SysYASTPrinter : public SysYBaseListener {
  std::ostream &os;
  int indent;

public:
  SysYASTPrinter(std::ostream &os = std::cout) : os(os), indent(0) {}

protected:
  void enter(const std::string &name) {
    os << std::string(indent, ' ') << name << '\n';
    indent += 2;
  }
  void exit() { indent -= 2; }

public:
  virtual void enterModule(SysYParser::ModuleContext * /*ctx*/) override {
    enter("module");
  }

  virtual void enterDecl(SysYParser::DeclContext * /*ctx*/) override {
    enter("decl");
  }

  virtual void enterBtype(SysYParser::BtypeContext * /*ctx*/) override {
    enter("btype");
  }

  // virtual void
  // enterConstValue(SysYParser::ConstValueContext * /*ctx*/) override {
  //   enter("constValue");
  // }

  virtual void enterVarDef(SysYParser::VarDefContext *ctx) override {
    enter("varDef");
  }

  virtual void enterFunc(SysYParser::FuncContext * /*ctx*/) override {
    enter("func");
  }

  virtual void enterFuncType(SysYParser::FuncTypeContext * /*ctx*/) override {
    enter("funcType");
  }

  virtual void
  enterFuncFParams(SysYParser::FuncFParamsContext * /*ctx*/) override {
    enter("funcFParams");
  }

  virtual void
  enterFuncFParam(SysYParser::FuncFParamContext * /*ctx*/) override {
    enter("funcFParam");
  }

  virtual void enterBlockStmt(SysYParser::BlockStmtContext * /*ctx*/) override {
    enter("blockStmt");
  }

  virtual void enterBlockItem(SysYParser::BlockItemContext * /*ctx*/) override {
    enter("blockIterm");
  }

  virtual void enterStmt(SysYParser::StmtContext * /*ctx*/) override {
    enter("stmt");
  }

  virtual void
  enterAssignStmt(SysYParser::AssignStmtContext * /*ctx*/) override {
    enter("assignStmt");
  }

  virtual void enterExpStmt(SysYParser::ExpStmtContext * /*ctx*/) override {
    enter("expStmt");
  }

  virtual void enterIfStmt(SysYParser::IfStmtContext *ctx) override {
    enter("ifStmt " + ctx->getText());
  }

  virtual void enterWhileStmt(SysYParser::WhileStmtContext * /*ctx*/) override {
    enter("whileStmt");
  }

  virtual void enterBreakStmt(SysYParser::BreakStmtContext * /*ctx*/) override {
    enter("break");
  }

  virtual void
  enterContinueStmt(SysYParser::ContinueStmtContext * /*ctx*/) override {
    enter("continueStmt");
  }

  virtual void
  enterReturnStmt(SysYParser::ReturnStmtContext * /*ctx*/) override {
    enter("returnStmt");
  }

  virtual void enterEmptyStmt(SysYParser::EmptyStmtContext * /*ctx*/) override {
    enter("emptyStmt");
  }

  virtual void enterRelationExp(SysYParser::RelationExpContext *ctx) override {
    enter(std::string("relationExp: ") + ctx->getText());
  }

  virtual void
  enterMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override {
    enter(std::string("multiplicativeExp: ") + ctx->getText());
  }

  virtual void enterCallExp(SysYParser::CallExpContext * ctx) override {
    enter("callExp " + ctx->getText());
  }
  virtual void enterLValueExp(SysYParser::LValueExpContext * /*ctx*/) override {
    enter("lValueExp");
  }

  virtual void enterNumberExp(SysYParser::NumberExpContext * /*ctx*/) override {
    enter("numberExp");
  }

  virtual void enterAndExp(SysYParser::AndExpContext * /*ctx*/) override {
    enter("andExp");
  }

  virtual void enterUnaryExp(SysYParser::UnaryExpContext *ctx) override {
    enter(std::string("multiplicativeExp: ") + ctx->getText());
  }

  virtual void enterParenExp(SysYParser::ParenExpContext * /*ctx*/) override {
    enter("parenExp");
  }

  virtual void enterStringExp(SysYParser::StringExpContext * /*ctx*/) override {
    enter("stringExp");
  }

  virtual void enterOrExp(SysYParser::OrExpContext * /*ctx*/) override {
    enter("orExp");
  }

  virtual void enterAdditiveExp(SysYParser::AdditiveExpContext *ctx) override {
    enter(std::string("additiveExp: ") + ctx->getText());
  }

  virtual void enterEqualExp(SysYParser::EqualExpContext *ctx) override {
    enter(std::string("equalExp: ") + ctx->getText());
  }

  virtual void enterLValue(SysYParser::LValueContext * /*ctx*/) override {
    enter("lValue");
  }

  virtual void enterNumber(SysYParser::NumberContext * /*ctx*/) override {
    enter("number");
  }

  virtual void enterString(SysYParser::StringContext * /*ctx*/) override {
    enter("string");
  }

  virtual void
  enterFuncRParams(SysYParser::FuncRParamsContext * /*ctx*/) override {
    enter("funcRparams");
  }

  virtual void enterEveryRule(antlr4::ParserRuleContext *ctx) override {
    // os << ctx->getText();
    // enter("");
  }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override {
    exit();
  }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override {}
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override {}
};
