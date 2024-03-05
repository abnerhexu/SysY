#include "ASTPrinter.h"
#include "SysYParser.h"

extern std::size_t indentLevel;

void printIndent(){
  for (std::size_t i = 0; i < indentLevel; ++i){
    std::cout << "\t";
  }
}

std::any ASTPrinter::visitNumber(SysYParser::NumberContext *ctx) {
  if (ctx->IntConst()){
    std::cout << ctx->IntConst()->getText();
  }
  else if (ctx->FloatConst()){
    std::cout << ctx->FloatConst()->getText();
  }
  return nullptr;
}

std::any ASTPrinter::visitString(SysYParser::StringContext *ctx) {
  std::cout << ctx->String()->getText();
  return nullptr;
}

std::any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
  if (ctx->expr().empty())
    return nullptr;
  auto numParams = ctx->expr().size();
  ctx->expr(0)->accept(this);
  for (int i = 1; i < numParams; ++i) {
    std::cout << ", ";
    ctx->expr(i)->accept(this);
  }
  std::cout << '\n';
  return nullptr;
}

std::any ASTPrinter::visitFuncType(SysYParser::FuncTypeContext *ctx) {
  if (ctx->Void()){
    std::cout << ctx->Void()->getText();
  }
  else if (ctx->Int()){
    std::cout << ctx->Int()->getText();
  }
  else if (ctx->Float()){
    std::cout << ctx->Float()->getText();
  }
  return nullptr;
}

std::any ASTPrinter::visitCompUnit(SysYParser::CompUnitContext *ctx) {
  if (ctx->decl().empty() && ctx->funcDef().empty()){
    return nullptr;
  }
  std::size_t numDecls = ctx->decl().size();
  std::size_t numFuncDefs = ctx->funcDef().size();
  for (int i = 0; i < numDecls; i++) {
    visitDecl(ctx->decl(i));
    std::cout << std::endl;
  }
  std::cout << std::endl;
  for (int i = 0; i < numFuncDefs; i++) {
    visitFuncDef(ctx->funcDef(i));
    std::cout << std::endl;
    std::cout << std::endl;
  }
  return nullptr;
}

std::any ASTPrinter::visitDataType(SysYParser::DataTypeContext *ctx){
  if (ctx->Int()){
    std::cout << ctx->Int()->getText();
  }
  else if (ctx->Float()){
    std::cout << ctx->Float()->getText();
  }
  return nullptr;
}

std::any visitDecl(SysYParser::DeclContext *ctx) {
  antlr4::tree::ParseTree * child = ctx->children[0];
  auto * constChild = dynamic_cast<SysYParser::ConstDeclContext*>(child);
  auto * varChild = dynamic_cast<SysYParser::VarDeclContext*>(child);
  // TODO(Abner) using if (ctx->constDecl())?
  if (constChild) {
    visitConstDecl(ctx->constDecl());
  }
  else if (varChild) {
    visitVarDecl(ctx->varDecl());
  }
}

std::any visitConstDecl(SysYParser::ConstDeclContext*ctx) {
  std::cout << ctx->Const()->getText() << " ";
  visitDataType(ctx->dataType());
  std::cout << " ";
  std::size_t numConstDefs = ctx->constDef().size();
  for (int i = 0; i < numConstDefs; i++) {
    visitConstDef(ctx->constDef(i));
    if (numConstDefs > 1 && i < numConstDefs - 1) {
      // TODO(abner) 
      // std::cout << ctx->Comma(i)->getText() << " ";
      std::cout << ", ";
    }
  }
  // std::cout << ctx->Semi()->getText();
  std::cout << ";" << std::endl;
  return nullptr;
}
std::any visitConstDef(SysYParser::ConstDefContext*ctx) {
  std::cout << ctx->Identifier()->getText() << " ";
  std::size_t numConstExpr = ctx->constExpr().size();
  for (std::size_t i = 0; i < numConstExpr; i++) {
    std::cout << "[";
    visitConstExpr(ctx->constExpr(i));
    std::cout << "]";
  }
  std::cout << " " << ctx->Assign()->getText() << " ";
  visitConstInitVal(ctx->constInitVal());
  return nullptr;
}
std::any visitConstInitVal(SysYParser::ConstInitValContext *ctx) {
  auto* child = ctx->children[0];
  auto* expChild = dynamic_cast<SysYParser::ConstExprContext*>(child);
  if (expChild) {
    visitConstExpr(ctx->constExpr());
  } 
  else if (ctx->LEFT_BRACE()){
    std::cout << "{";
    std::size_t numConstInitVal = ctx->constInitVal().size();
    for (int i = 0; i < numConstInitVal; i++) {
      visitConstInitVal(ctx->constInitVal(i));
      if (numConstInitVal > 1 && i < numConstInitVal - 1) {
        // TODO(abner) 
        // std::cout << ctx->Comma(i)->getText() << " ";
        std::cout << ", ";
      }
    }
    std::cout << "}";
  }
  return nullptr;
}
std::any visitVarDecl(SysYParser::VarDeclContext*ctx) {
  visitDataType(ctx->dataType());
  std::cout << " ";
  std::size_t numVarDef = ctx->varDef().size();
  for (std::size_t i = 0; i < numVarDef; i++) {
    visitVarDef(ctx->varDef(i));
    if (numVarDef > 1 && i < numVarDef - 1) {
      std::cout << ", ";
    }
  }
  std::cout << ";";
  return nullptr;
}
std::any visitVarDef(SysYParser::VarDefContext *ctx) {
  std::cout << ctx->Identifier()->getText() << " ";
  std::size_t numConstExpr = ctx->constExpr().size();
  for (std::size_t i = 0; i < numConstExpr; i++) {
    std::cout << "[";
    visitConstExpr(ctx->constExpr(i));
    std::cout << "]";
  }
  if (ctx->Assign()){
    std::cout << " = ";
    visitInitVal(ctx->initVal());
  }
  return nullptr;
}
std::any visitInitVal(SysYParser::InitValContext*ctx) {
  auto* child = ctx->children[0];
  auto exprChild = dynamic_cast<SysYParser::ExprContext*>(child);
  if (exprChild) {
    visitExpr(ctx->expr());
  }
  else if (ctx->LEFT_BRACE()){
    std::cout << "{";
    std::size_t numInitVal = ctx->initVal().size();
    for (std::size_t i = 0; i < numInitVal; i++) {
      visitInitVal(ctx->initVal(i));
      if (numInitVal > 1 && i < numInitVal - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "}";
  }
  return nullptr;
}
std::any visitFuncDef(SysYParser::FuncDefContext*ctx) {
  visitFuncType(ctx->funcType());
  std::cout << " ";
  std::cout << ctx->Identifier()->getText() << "(";
  if (ctx->funcFParams()){
    visitFuncFParams(ctx->funcFParams());
  }
  std::cout << ") ";
  indentLevel++;
  visitBlockStmt(ctx->blockStmt());
  indentLevel--;
  return nullptr;
}

std::any visitFuncType(SysYParser::FuncTypeContext*ctx) {
  if (ctx->Void()){
    std::cout << ctx->Void()->getText();
  }
  else if (ctx->Int()){
    std::cout << ctx->Int()->getText();
  }
  else if (ctx->Float()){
    std::cout << ctx->Float()->getText();
  }
  return nullptr;
}
std::any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
  std::size_t numFuncFParam = ctx->funcFParam().size();
  for (std::size_t i = 0; i < numFuncFParam; i++) {
    visitFuncFParam(ctx->funcFParam(i));
    if (numFuncFParam > 1 && i < numFuncFParam - 1) {
      std::cout << " ";
    }
  }
  return nullptr;
}
std::any visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
  visitDataType(ctx->dataType());
  std::cout << " ";
  std::cout << ctx->Identifier()->getText();
  std::size_t numBracket = ctx->LEFT_BRACKET().size();
  if (numBracket == 1){
    std::cout << ctx->LEFT_BRACKET(0)->getText();
    std::cout << ctx->RIGHT_BRACKET(0)->getText();
  }
  else {
    std::cout << ctx->LEFT_BRACKET(0)->getText();
    std::cout << ctx->RIGHT_BRACKET(0)->getText();
    for (std::size_t i = 1; i < numBracket; i++) {
      std::cout << ctx->LEFT_BRACKET(i)->getText();
      visitExpr(ctx->expr(i));
      std::cout << ctx->RIGHT_BRACKET(i)->getText();
    }
  }
  return nullptr;
}
std::any visitBlockStmt(SysYParser::BlockStmtContext*ctx) {
  std::cout << "{" << std::endl;
  printIndent();
  std::size_t numBlockItem = ctx->blockItem().size();
  for (std::size_t i = 0; i < numBlockItem; i++) {
    visitBlockItem(ctx->blockItem(i));
    if (i < numBlockItem - 1 && numBlockItem > 1) {
      std::cout << std::endl;
      printIndent();
    }
  }
  std::cout << std::endl;
  printIndent();
  std::cout << "}" << std::endl;
  return nullptr;
}
std::any visitBlockItem(SysYParser::BlockItemContext*ctx) {
  auto* child = ctx->children[0];
  auto* declChild = dynamic_cast<SysYParser::DeclContext*>(child);
  auto* stmtChild = dynamic_cast<SysYParser::StmtContext*>(child);
  if (declChild) {
    visitDecl(ctx->decl());
  }
  else if (stmtChild) {
    visitStmt(ctx->stmt());
  }
  return nullptr;
}
std::any visitStmt(SysYParser::StmtContext *ctx) {
  auto * child = ctx->children[0];
  auto* lhsValChild = dynamic_cast<SysYParser::LhsValContext*>(child);
  auto* blockStmtChild = dynamic_cast<SysYParser::BlockStmtContext*>(child);
  auto* exprChild = dynamic_cast<SysYParser::ExprContext*>(child);
  if (lhsValChild) {
    visitLhsVal(ctx->lhsVal());
    std::cout << " = ";
    visitExpr(ctx->expr());
    std::cout << ";" << std::endl;
  }
  else if (blockStmtChild) {
    indentLevel++;
    visitBlockStmt(ctx->blockStmt());
    indentLevel--;
  }
  else if (ctx->If()){
    std::cout << "if (";
    visitCond(ctx->cond());
    std::cout << ")";
    auto* tmpStmt = ctx->stmt(0)->children[0];
    auto* terminalStmt = dynamic_cast<antlr4::tree::TerminalNode*>(tmpStmt);
    auto* subLhsChild = dynamic_cast<SysYParser::LhsValContext*>(tmpStmt);
    auto* subExprChild = dynamic_cast<SysYParser::ExprContext*>(tmpStmt);
    if (terminalStmt || subExprChild || subLhsChild) {
      std::cout << "{" << std::endl;
      indentLevel++;
      printIndent();
      visitStmt(ctx->stmt(0));
      printIndent();
      std::cout << "}";
      indentLevel--;
    }
    else {
      std::cout << ";" << std::endl; // TODO(abner) bug fix needed
    }
    if (ctx->Else()){
      std::cout << std::endl;
      printIndent();
      std::cout << "else";
      tmpStmt = ctx->stmt(1)->children[0];
      terminalStmt = dynamic_cast<antlr4::tree::TerminalNode*>(tmpStmt);
      subLhsChild = dynamic_cast<SysYParser::LhsValContext*>(tmpStmt);
      subExprChild = dynamic_cast<SysYParser::ExprContext*>(tmpStmt);
      if (terminalStmt || subExprChild || subLhsChild) {
        std::cout << "{" << std::endl;
        indentLevel++;
        printIndent();
        visitStmt(ctx->stmt(0));
        printIndent();
        std::cout << "}";
        indentLevel--;
      }
      else {
        std::cout << ";" << std::endl; // TODO(abner) bug fix needed
      }
    }
  }
  else if (ctx->While()){
    std::cout << ctx->While()->getText() << "(";
    visitCond(ctx->cond());
    std::cout << ")";
    auto* tmpStmt = ctx->stmt(0)->children[0];
    auto* terminalStmt = dynamic_cast<antlr4::tree::TerminalNode*>(tmpStmt);
    auto* subLhsChild = dynamic_cast<SysYParser::LhsValContext*>(tmpStmt);
    auto* subExprChild = dynamic_cast<SysYParser::ExprContext*>(tmpStmt);
    if (terminalStmt || subExprChild || subLhsChild) {
      std::cout << "{" << std::endl;
      indentLevel++;
      printIndent();
      visitStmt(ctx->stmt(0));
      std::cout << std::endl;
      printIndent();
      std::cout << "}";
      indentLevel--;
    }
    else {
      visitStmt(ctx->stmt(0));
    }
  }
  else if (ctx->Break()){
    std::cout << ctx->Break()->getText() << ctx->Semi()->getText() << std::endl;
  }
  else if (ctx->Continue()){
    std::cout << ctx->Continue()->getText() << ctx->Semi()->getText() << std::endl;
  }
  else if (ctx->Return()){
    std::cout << ctx->Return()->getText() << " ";
    if (ctx->expr()){
      visitExpr(ctx->expr());
    }
    std::cout << ";" << std::endl;
  }
  else {
    std::cout << ctx->Semi()->getText();
  }
  return nullptr;
}
std::any visitExpr(SysYParser::ExprContext *ctx) {
  visitAddExpr(ctx->addExpr());
  return nullptr;
}
std::any visitCond(SysYParser::CondContext *ctx) {

}
std::any visitLhsVal(SysYParser::LhsValContext *ctx);
std::any visitPrimaryExp(SysYParser::PrimaryExprContext *ctx);
std::any visitNumber(SysYParser::NumberContext *ctx);
std::any visitString(SysYParser::StringContext *ctx);
std::any visitUnaryExpr(SysYParser::UnaryExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* primaryExpr = dynamic_cast<SysYParser::PrimaryExprContext*>(child);
  auto* unaryExpr = dynamic_cast<SysYParser::UnaryExprContext*>(child);
  if (primaryExpr){
    visitPrimaryExp(primaryExpr);
  }
  else if (ctx->Identifier()){
    std::cout << ctx->Identifier()->getText() << std::endl;
    std::cout << ctx->LEFT_PAREN()->getText() << std::endl;
    if (ctx->funcRParams()){
      visitFuncRParams(ctx->funcRParams());
    }
    
  }
}
std::any visitUnaryOp(SysYParser::UnaryOpContext*ctx);
std::any visitFuncRParam(SysYParser::FuncRParamsContext *ctx);
std::any visitMulExpr(SysYParser::MulExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* mulChild = dynamic_cast<SysYParser::MulExprContext*>(child);
  auto* unaryChild = dynamic_cast<SysYParser::UnaryExprContext*>(child);
  if (mulChild) {
    visitMulExpr(ctx->mulExpr());
    if (ctx->Mul()){
      std::cout << ctx->Mul()->getText() << " ";
    }
    else if (ctx->Div()){
      std::cout << ctx->Div()->getText() << " ";
    }
    else if (ctx->Mod()){
      std::cout << ctx->Mod()->getText() << " ";
    }
    visitUnaryExpr(ctx->unaryExpr());
  }
  else if (unaryChild){
    visitUnaryExpr(ctx->unaryExpr());
  }
  return nullptr;
}
std::any visitAddExpr(SysYParser::AddExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* addChild = dynamic_cast<SysYParser::AddExprContext*>(child);
  auto mulChild = dynamic_cast<SysYParser::MulExprContext*>(child);
  if (addChild) {
    visitAddExpr(ctx->addExpr());
    if (ctx->Add()){
      std::cout << ctx->Add()->getText() << " ";
    }
    else if (ctx->Sub()){
      std::cout << ctx->Sub()->getText() << " ";
    }
    visitMulExpr(ctx->mulExpr());
  }
  else if (mulChild){
    visitMulExpr(ctx->mulExpr());
  }
  return nullptr;
}
std::any visitRelExpr(SysYParser::RelExprContext*ctx);
std::any visitEqExpr(SysYParser::EqExprContext *ctx);
std::any visitLAndExpr(SysYParser::LAndExprContext*ctx);
std::any visitLOrExpr(SysYParser::LOrExprContext*ctx);
std::any visitConstExpr(SysYParser::ConstExprContext*ctx);