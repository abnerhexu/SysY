#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
#include "SysYParser.h"
#include "ASTPrinter.h"

antlrcpp::Any ASTPrinter::visitNumber(SysYParser::NumberContext *ctx) {
  if (auto iCtx = ctx->IntLiteral()) {
    cout << iCtx->getText();
  } else if (auto fCtx = ctx->FloatLiteral()) {
    cout << fCtx->getText();
  }
  return nullptr;
}

antlrcpp::Any ASTPrinter::visitStringAsRParam(SysYParser::StringAsRParamContext *ctx) {
  cout << ctx->getText();
  return nullptr;
}

// antlrcpp::Any ASTPrinter::visitFuncRParam(SysYParser::FuncRParamContext *ctx) {
//   if (auto exp = dynamic_cast<SysYParser::ExpAsRParamContext *>(ctx))
//     return visitExpAsRParam(exp);
//   else
//     return visitStringAsRParam(static_cast<SysYParser::StringAsRParamContext
//     *>(ctx));
// }

antlrcpp::Any ASTPrinter::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
  if (ctx->funcRParam().empty())
    return nullptr;
  ctx->funcRParam(0)->accept(this);
  auto numParams = ctx->funcRParam().size();
  for (int i = 1; i < numParams; ++i) {
    cout << ", ";
    ctx->funcRParam(i)->accept(this);
  }
  return nullptr;
}