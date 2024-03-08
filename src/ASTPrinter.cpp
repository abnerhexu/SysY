#include "ASTPrinter.h"
#include "SysYParser.h"

std::size_t indentLevel;

void printIndent(int level){
  for (auto i = 0; i < level; ++i){
    for (int j = 0; j < 2; j++) std::cout << " ";
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


// std::any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
//   if (ctx->expr().empty())
//     return nullptr;
//   auto numParams = ctx->expr().size();
//   ctx->expr(0)->accept(this);
//   for (int i = 1; i < numParams; ++i) {
//     std::cout << ", ";
//     ctx->expr(i)->accept(this);
//   }
//   std::cout << '\n';
//   return nullptr;
// }

std::any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
  auto numExpr = ctx->expr().size();
  for (int i = 0; i < numExpr; i++) {
    visitExpr(ctx->expr(i));
    if (i < numExpr - 1 && numExpr > 1){
      std::cout << ", ";
    }
  }
  return nullptr;
}

std::any ASTPrinter::visitCompUnit(SysYParser::CompUnitContext *ctx) {
  if (ctx->decl().empty() && ctx->funcDef().empty()){
    return nullptr;
  }
  auto numDecls = ctx->decl().size();
  auto numFuncDefs = ctx->funcDef().size();
  for (int i = 0; i < numDecls; i++) {
    visitDecl(ctx->decl(i));
    std::cout << std::endl;
  }
  std::cout << std::endl;
  for (int i = 0; i < numFuncDefs; i++) {
    visitFuncDef(ctx->funcDef(i));
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

std::any ASTPrinter::visitDecl(SysYParser::DeclContext *ctx) {
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
  return nullptr;
}

std::any ASTPrinter::visitConstDecl(SysYParser::ConstDeclContext*ctx) {
  std::cout << ctx->Const()->getText() << " ";
  visitDataType(ctx->dataType());
  std::cout << " ";
  auto numConstDefs = ctx->constDef().size();
  for (int i = 0; i < numConstDefs; i++) {
    visitConstDef(ctx->constDef(i));
    if (numConstDefs > 1 && i < numConstDefs - 1) {
      // TODO(abner) 
      // std::cout << ctx->Comma(i)->getText() << " ";
      std::cout << ", ";
    }
  }
  // std::cout << ctx->Semi()->getText();
  std::cout << ";";
  return nullptr;
}
std::any ASTPrinter::visitConstDef(SysYParser::ConstDefContext*ctx) {
  std::cout << ctx->Identifier()->getText();
  auto numConstExpr = ctx->constExpr().size();
  for (auto i = 0; i < numConstExpr; i++) {
    std::cout << "[";
    visitConstExpr(ctx->constExpr(i));
    std::cout << "]";
  }
  std::cout << " " << ctx->Assign()->getText() << " ";
  visitConstInitVal(ctx->constInitVal());
  return nullptr;
}
std::any ASTPrinter::visitConstInitVal(SysYParser::ConstInitValContext *ctx) {
  auto* child = ctx->children[0];
  auto* expChild = dynamic_cast<SysYParser::ConstExprContext*>(child);
  if (expChild) {
    visitConstExpr(ctx->constExpr());
  } 
  else if (ctx->LEFT_BRACE()){
    std::cout << "{";
    auto numConstInitVal = ctx->constInitVal().size();
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
std::any ASTPrinter::visitVarDecl(SysYParser::VarDeclContext*ctx) {
  visitDataType(ctx->dataType());
  std::cout << " ";
  auto numVarDef = ctx->varDef().size();
  for (auto i = 0; i < numVarDef; i++) {
    visitVarDef(ctx->varDef(i));
    if (numVarDef > 1 && i < numVarDef - 1) {
      std::cout << ", ";
    }
  }
  std::cout << ";";
  return nullptr;
}
std::any ASTPrinter::visitVarDef(SysYParser::VarDefContext *ctx) {
  std::cout << ctx->Identifier()->getText();
  auto numConstExpr = ctx->constExpr().size();
  for (auto i = 0; i < numConstExpr; i++) {
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
std::any ASTPrinter::visitInitVal(SysYParser::InitValContext*ctx) {
  auto* child = ctx->children[0];
  auto exprChild = dynamic_cast<SysYParser::ExprContext*>(child);
  if (exprChild) {
    visitExpr(ctx->expr());
  }
  else if (ctx->LEFT_BRACE()){
    std::cout << "{";
    auto numInitVal = ctx->initVal().size();
    for (auto i = 0; i < numInitVal; i++) {
      visitInitVal(ctx->initVal(i));
      if (numInitVal > 1 && i < numInitVal - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "}";
  }
  return nullptr;
}
std::any ASTPrinter::visitFuncDef(SysYParser::FuncDefContext*ctx) {
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

std::any ASTPrinter::visitFuncType(SysYParser::FuncTypeContext*ctx) {
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
std::any ASTPrinter::visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
  if (ctx->funcFParam().empty()) {
    return nullptr;
  }
  auto numFuncFParam = ctx->funcFParam().size();
  for (auto i = 0; i < numFuncFParam; i++) {
    visitFuncFParam(ctx->funcFParam(i));
    if (numFuncFParam > 1 && i < numFuncFParam - 1) {
      std::cout << ", ";
    }
  }
  return nullptr;
}
std::any ASTPrinter::visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
  visitDataType(ctx->dataType());
  std::cout << " ";
  std::cout << ctx->Identifier()->getText();
  if (ctx->LEFT_BRACKET().empty()) {
    return nullptr;
  }
  auto numBracket = ctx->LEFT_BRACKET().size();
  if (numBracket == 1){
    std::cout << ctx->LEFT_BRACKET(0)->getText();
    std::cout << ctx->RIGHT_BRACKET(0)->getText();
  }
  else {
    std::cout << ctx->LEFT_BRACKET(0)->getText();
    std::cout << ctx->RIGHT_BRACKET(0)->getText();
    for (auto i = 1; i < numBracket; i++) {
      std::cout << ctx->LEFT_BRACKET(i)->getText();
      visitExpr(ctx->expr(i));
      std::cout << ctx->RIGHT_BRACKET(i)->getText();
    }
  }
  return nullptr;
}
std::any ASTPrinter::visitBlockStmt(SysYParser::BlockStmtContext*ctx) {
  std::cout << "{" << std::endl;
  printIndent(indentLevel);
  if (ctx->blockItem().empty()) {
    return nullptr;
  }
  auto numBlockItem = ctx->blockItem().size();
  for (auto i = 0; i < numBlockItem; i++) {
    visitBlockItem(ctx->blockItem(i));
    if (i < numBlockItem - 1 && numBlockItem > 1) {
      std::cout << std::endl;
      printIndent(indentLevel);
    }
  }
  std::cout << std::endl;
  printIndent(indentLevel-1);
  std::cout << "}";
  return nullptr;
}
std::any ASTPrinter::visitBlockItem(SysYParser::BlockItemContext*ctx) {
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
std::any ASTPrinter::visitStmt(SysYParser::StmtContext *ctx) {
  auto * child = ctx->children[0];
  auto* lhsValChild = dynamic_cast<SysYParser::LhsValContext*>(child);
  auto* blockStmtChild = dynamic_cast<SysYParser::BlockStmtContext*>(child);
  auto* exprChild = dynamic_cast<SysYParser::ExprContext*>(child);
  if (lhsValChild) {
    visitLhsVal(ctx->lhsVal());
    std::cout << " = ";
    visitExpr(ctx->expr());
    std::cout << ";";
  }
  else if (exprChild){
    visitExpr(ctx->expr());
    std::cout << ";";
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
      printIndent(indentLevel);
      visit(ctx->stmt(0));
      std::cout << std::endl;
      printIndent(indentLevel-1);
      std::cout << "}";
      indentLevel--;
    }
    else {
      visitStmt(ctx->stmt(0)); // TODO(abner) bug fix needed
    }
    if (ctx->Else()){
      std::cout << std::endl;
      printIndent(indentLevel);
      std::cout << "else";
      tmpStmt = ctx->stmt(1)->children[0];
      terminalStmt = dynamic_cast<antlr4::tree::TerminalNode*>(tmpStmt);
      subLhsChild = dynamic_cast<SysYParser::LhsValContext*>(tmpStmt);
      subExprChild = dynamic_cast<SysYParser::ExprContext*>(tmpStmt);
      if (terminalStmt || subExprChild || subLhsChild) {
        std::cout << "{" << std::endl;
        indentLevel++;
        printIndent(indentLevel);
        visit(ctx->stmt(0));
        printIndent(indentLevel-1);
        std::cout << "}";
        indentLevel--;
      }
      else {
        visitStmt(ctx->stmt(1)); // TODO(abner) bug fix needed
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
      printIndent(indentLevel);
      visit(ctx->stmt(0));
      std::cout << std::endl;
      printIndent(indentLevel-1);
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
    std::cout << ctx->Semi()->getText();
  }
  else {
    std::cout << ctx->Semi()->getText();
  }
  return nullptr;
}

std::any ASTPrinter::visitExpr(SysYParser::ExprContext *ctx) {
  visitAddExpr(ctx->addExpr());
  return nullptr;
}

std::any ASTPrinter::visitCond(SysYParser::CondContext *ctx) {
  visitLOrExpr(ctx->lOrExpr());
  return nullptr;
}
std::any ASTPrinter::visitLhsVal(SysYParser::LhsValContext *ctx) {
  std::cout << ctx->Identifier()->getText();
  if (ctx->expr().empty()){
    return nullptr;
  }
  int numExpr = ctx->expr().size();
  for (int i = 0; i < numExpr; i++) {
    std::cout << "[";
    visitExpr(ctx->expr(i));
    std::cout << "]";
  }
  return nullptr;
}

std::any ASTPrinter::visitPrimaryExpr(SysYParser::PrimaryExprContext *ctx) {
  auto* child = ctx->children[0];
  auto* lhsChild = dynamic_cast<SysYParser::LhsValContext*>(child);
  auto* numberChild = dynamic_cast<SysYParser::NumberContext*>(child);
  if (ctx->LEFT_PAREN()){
    std::cout << ctx->LEFT_PAREN()->getText();
    visitExpr(ctx->expr());
    std::cout << ctx->RIGHT_PAREN()->getText(); // << std::endl;
  }
  else if (lhsChild){
    visitLhsVal(ctx->lhsVal());
  }
  else if (numberChild) {
    visitNumber(ctx->number());
  }
  return nullptr;
}

std::any ASTPrinter::visitString(SysYParser::StringContext *ctx) {
  std::cout << ctx->String()->getText();
  return nullptr;
}

std::any ASTPrinter::visitUnaryExpr(SysYParser::UnaryExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* primaryExprChild = dynamic_cast<SysYParser::PrimaryExprContext*>(child);
  auto* unaryExprChild = dynamic_cast<SysYParser::UnaryExprContext*>(child);
  if (primaryExprChild){
    visitPrimaryExpr(primaryExprChild);
  }
  else if (ctx->Identifier()){
    std::cout << ctx->Identifier()->getText();
    std::cout << ctx->LEFT_PAREN()->getText();
    if (ctx->funcRParams()){
      visitFuncRParams(ctx->funcRParams());
    }
    std::cout << ctx->RIGHT_PAREN()->getText();
  }
  else if (unaryExprChild) {
    visitUnaryOp(ctx->unaryOp());
    visitUnaryExpr(ctx->unaryExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitUnaryOp(SysYParser::UnaryOpContext*ctx) {
  if (ctx->Add()) {
    std::cout << ctx->Add()->getText();
  }
  else if (ctx->Sub()) {
    std::cout << ctx->Sub()->getText();
  }
  else if (ctx->Not()) {
    std::cout << ctx->Not()->getText();
  }
  return nullptr;
}

std::any ASTPrinter::visitMulExpr(SysYParser::MulExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* mulChild = dynamic_cast<SysYParser::MulExprContext*>(child);
  auto* unaryChild = dynamic_cast<SysYParser::UnaryExprContext*>(child);
  if (mulChild) {
    visitMulExpr(ctx->mulExpr());
    if (ctx->Mul()){
      std::cout << " " << ctx->Mul()->getText() << " ";
    }
    else if (ctx->Div()){
      std::cout << " " << ctx->Div()->getText() << " ";
    }
    else if (ctx->Mod()){
      std::cout << " " << ctx->Mod()->getText() << " ";
    }
    visitUnaryExpr(ctx->unaryExpr());
  }
  else if (unaryChild){
    visitUnaryExpr(ctx->unaryExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitAddExpr(SysYParser::AddExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* addChild = dynamic_cast<SysYParser::AddExprContext*>(child);
  auto mulChild = dynamic_cast<SysYParser::MulExprContext*>(child);
  if (addChild) {
    visitAddExpr(ctx->addExpr());
    if (ctx->Add()){
      std::cout << " " << ctx->Add()->getText() << " ";
    }
    else if (ctx->Sub()){
      std::cout << " " << ctx->Sub()->getText() << " ";
    }
    visitMulExpr(ctx->mulExpr());
  }
  else if (mulChild){
    visitMulExpr(ctx->mulExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitRelExpr(SysYParser::RelExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* relChild = dynamic_cast<SysYParser::RelExprContext*>(child);
  auto* addChild = dynamic_cast<SysYParser::AddExprContext*>(child);
  if (relChild) {
    visitRelExpr(ctx->relExpr());
    if (ctx->Lt()){
      std::cout << " " << ctx->Lt()->getText() << " ";
    }
    else if (ctx->Le()){
      std::cout << " " << ctx->Le()->getText() << " ";
    }
    else if (ctx->Gt()){
      std::cout << " " << ctx->Gt()->getText() << " ";
    }
    else if (ctx->Ge()){
      std::cout << " " << ctx->Ge()->getText() << " ";
    }
    visitAddExpr(ctx->addExpr());
  }
  else if (addChild){
    visitAddExpr(ctx->addExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitEqExpr(SysYParser::EqExprContext *ctx) {
  auto* child = ctx->children[0];
  auto* relChild = dynamic_cast<SysYParser::RelExprContext*>(child);
  auto* eqChild = dynamic_cast<SysYParser::EqExprContext*>(child);
  if (eqChild) {
    visitEqExpr(ctx->eqExpr());
    if (ctx->Eq()){
      std::cout << " " << ctx->Eq()->getText() << " ";
    }
    else if (ctx->Ne()){
      std::cout << " " << ctx->Ne()->getText() << " ";
    }
    visitRelExpr(ctx->relExpr());
  }
  else if (relChild) {
    visitRelExpr(ctx->relExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitLAndExpr(SysYParser::LAndExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* eqChild = dynamic_cast<SysYParser::EqExprContext*>(child);
  auto* andChild = dynamic_cast<SysYParser::LAndExprContext*>(child);
  if (andChild) {
    visitLAndExpr(ctx->lAndExpr());
    std::cout << " " << ctx->And()->getText() << " ";
    visitEqExpr(ctx->eqExpr());
  }
  else if (eqChild) {
    visitEqExpr(ctx->eqExpr());
  }
  return nullptr;
}
std::any ASTPrinter::visitLOrExpr(SysYParser::LOrExprContext*ctx) {
  auto* child = ctx->children[0];
  auto* andChild = dynamic_cast<SysYParser::LAndExprContext*>(child);
  auto* orChild = dynamic_cast<SysYParser::LOrExprContext*>(child);
  if (orChild) {
    visitLOrExpr(ctx->lOrExpr());
    std::cout << ctx->Or()->getText() << " ";
    visitLAndExpr(ctx->lAndExpr());
  }
  else if (andChild){
    visitLAndExpr(ctx->lAndExpr());
  }
  return nullptr;
}

std::any ASTPrinter::visitConstExpr(SysYParser::ConstExprContext*ctx) {
  visitAddExpr(ctx->addExpr());
  return nullptr;
}