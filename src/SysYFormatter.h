#pragma once

#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include <ostream>

namespace sysy {

class SysYFormatter : public SysYBaseVisitor {
protected:
  std::ostream &os;
  int indent = 0;

public:
  SysYFormatter(std::ostream &os) : os(os), indent(0) {}

protected:
  struct Indentor {
    static constexpr int TabSize = 2;
    int &indent;
    Indentor(int &indent) : indent(indent) { indent += TabSize; }
    ~Indentor() { indent -= TabSize; }
  };
  std::ostream &space() { return os << std::string(indent, ' '); }
  template <typename T>
  std::ostream &interleave(const T &container, const std::string sep = ", ") {
    auto b = container.begin(), e = container.end();
    (*b)->accept(this);
    for (b = std::next(b); b != e; b = std::next(b)) {
      os << sep;
      (*b)->accept(this);
    }
    return os;
  }

public:
//   virtual std::any visitModule(SysYParser::ModuleContext *ctx) override {
//     return visitChildren(ctx);
//   }

  virtual std::any visitBtype(SysYParser::BtypeContext *ctx) override {
    os << ctx->getText();
    return 0;
  }

  virtual std::any visitDecl(SysYParser::DeclContext *ctx) override {
    space();
    if (ctx->CONST())
      os << ctx->CONST()->getText() << ' ';
    ctx->btype()->accept(this);
    os << ' ';
    interleave(ctx->varDef(), ", ") << ';' << '\n';
    return 0;
  }

  virtual std::any visitVarDef(SysYParser::VarDefContext *ctx) override {
    ctx->lValue()->accept(this);
    if (ctx->initValue()) {
      os << ' ' << '=' << ' ';
      ctx->initValue()->accept(this);
    }
    return 0;
  }

  virtual std::any
  visitArrayInitValue(SysYParser::ArrayInitValueContext *ctx) override {
    os << '{';
    auto values = ctx->initValue();
    if (values.size())
      interleave(values, ", ");
    os << '}';
    return 0;
  }

  virtual std::any visitFunc(SysYParser::FuncContext *ctx) override {
    ctx->funcType()->accept(this);
    os << ' ' << ctx->ID()->getText() << '(';
    if (ctx->funcFParams())
      ctx->funcFParams()->accept(this);
    os << ')' << ' ';
    ctx->blockStmt()->accept(this);
    os << '\n';
    return 0;
  }

  virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override {
    os << ctx->getText();
    return 0;
  }

  virtual std::any
  visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override {
    interleave(ctx->funcFParam(), ", ");
    return 0;
  }

  virtual std::any
  visitFuncFParam(SysYParser::FuncFParamContext *ctx) override {
    ctx->btype()->accept(this);
    os << ' ' << ctx->ID()->getText();
    if (not ctx->LBRACKET().empty()) {
      os << '[';
      auto exp = ctx->exp();
      if (not exp.empty()) {
        os << '[';
        interleave(exp, "][") << ']';
      }
    }
    return 0;
  }

  virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override {
    os << '{' << '\n';
    {
      Indentor indentor(indent);
      auto items = ctx->blockItem();
      if (not items.empty())
        interleave(items, "");
    }
    space() << ctx->RBRACE()->getText() << '\n';
    return 0;
  }

  //   virtual std::any visitBlockItem(SysYParser::BlockItemContext *ctx)
  //   override {
  //     return visitChildren(ctx);
  //   }

  //   virtual std::any visitStmt(SysYParser::StmtContext *ctx) override {
  //     return visitChildren(ctx);
  //   }

  virtual std::any
  visitAssignStmt(SysYParser::AssignStmtContext *ctx) override {
    space();
    ctx->lValue()->accept(this);
    os << " = ";
    ctx->exp()->accept(this);
    os << ';' << '\n';
    return 0;
  }

  virtual std::any visitExpStmt(SysYParser::ExpStmtContext *ctx) override {
    space();
    ctx->exp()->accept(this);
    os << ';' << '\n';
    return 0;
  }

  void wrapBlock(SysYParser::StmtContext *stmt) {
    bool isBlock = stmt->blockStmt();
    if (isBlock) {
      stmt->accept(this);
    } else {
      os << "{\n";
      {
        Indentor indentor(indent);
        stmt->accept(this);
      }
      space() << "}\n";
    }
  };
  virtual std::any visitIfStmt(SysYParser::IfStmtContext *ctx) override {
    space();
    os << ctx->IF()->getText() << " (";
    ctx->exp()->accept(this);
    os << ") ";
    auto stmt = ctx->stmt();
    auto ifStmt = stmt[0];
    wrapBlock(ifStmt);
    if (stmt.size() == 2) {
      auto elseStmt = stmt[1];
      wrapBlock(elseStmt);
    }
    return 0;
  }

  virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override {
    space();
    os << ctx->WHILE()->getText() << " (";
    ctx->exp()->accept(this);
    os << ") ";
    wrapBlock(ctx->stmt());
    return 0;
  }

  virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override {
    space() << ctx->BREAK()->getText() << ';' << '\n';
    return 0;
  }

  virtual std::any
  visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override {
    space() << ctx->CONTINUE()->getText() << ';'
            << '\n';
    return 0;
  }

  virtual std::any
  visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override {
    space() << ctx->RETURN()->getText();
    if (ctx->exp()) {
      os << ' ';
      ctx->exp()->accept(this);
    }
    os << ';' << '\n';
    return 0;
  }

  //   virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx)
  //   override {
  //     return visitChildren(ctx);
  //   }

  virtual std::any
  visitRelationExp(SysYParser::RelationExpContext *ctx) override {
    auto lhs = ctx->exp(0);
    auto rhs = ctx->exp(1);
    std::string op =
        ctx->LT() ? "<" : (ctx->LE() ? "<=" : (ctx->GT() ? ">" : ">="));
    lhs->accept(this);
    os << ' ' << op << ' ';
    rhs->accept(this);
    return 0;
  }

  virtual std::any
  visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override {
    auto lhs = ctx->exp(0);
    auto rhs = ctx->exp(1);
    std::string op = ctx->MUL() ? "*" : (ctx->DIV() ? "/" : "%");
    lhs->accept(this);
    os << ' ' << op << ' ';
    rhs->accept(this);
    return 0;
  }

//   virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override {
//     return visitChildren(ctx);
//   }

//   virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override {
//     return visitChildren(ctx);
//   }

  virtual std::any visitAndExp(SysYParser::AndExpContext *ctx) override {
    ctx->exp(0)->accept(this);
    os << " && ";
    ctx->exp(1)->accept(this);
    return 0;
  }

  virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override {
    std::string op = ctx->ADD() ? "+" : (ctx->SUB() ? "-" : "!");
    os << op;
    ctx->exp()->accept(this);
    return 0;
  }

  virtual std::any visitParenExp(SysYParser::ParenExpContext *ctx) override {
    os << '(';
    ctx->exp()->accept(this);
    os << ')';
    return 0;
  }

  virtual std::any visitStringExp(SysYParser::StringExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrExp(SysYParser::OrExpContext *ctx) override {
    ctx->exp(0)->accept(this);
    os << " || ";
    ctx->exp(1)->accept(this);
    return 0;
  }

//   virtual std::any visitCallExp(SysYParser::CallExpContext *ctx) override {
//     return visitChildren(ctx);
//   }

  virtual std::any
  visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override {
    auto lhs = ctx->exp(0);
    auto rhs = ctx->exp(1);
    std::string op = ctx->ADD() ? "+" : "-";
    lhs->accept(this);
    os << ' ' << op << ' ';
    rhs->accept(this);
    return 0;
  }

  virtual std::any visitEqualExp(SysYParser::EqualExpContext *ctx) override {
    auto lhs = ctx->exp(0);
    auto rhs = ctx->exp(1);
    std::string op = ctx->EQ() ? "==" : "!=";
    lhs->accept(this);
    os << ' ' << op << ' ';
    rhs->accept(this);
    return 0;
  }

  virtual std::any visitCall(SysYParser::CallContext *ctx) override {
    os << ctx->ID()->getText() << '(';
    if (ctx->funcRParams())
      ctx->funcRParams()->accept(this);
    os << ')';
    return 0;
  }

  virtual std::any visitLValue(SysYParser::LValueContext *ctx) override {
    os << ctx->ID()->getText();
    auto exp = ctx->exp();
    if (not exp.empty()) {
      os << '[';
      interleave(exp, "][") << ']';
    }
    return 0;
  }

  virtual std::any visitNumber(SysYParser::NumberContext *ctx) override {
    os << ctx->getText();
    return 0;
  }

  virtual std::any visitString(SysYParser::StringContext *ctx) override {
    os << ctx->getText();
    return 0;
  }

  virtual std::any
  visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override {
    interleave(ctx->exp(), ", ");
    return 0;
  }
};

} // namespace sysy
