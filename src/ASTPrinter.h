#pragma once

#include "SysYBaseVisitor.h"

class ASTPrinter : public SysYBaseVisitor {
public:
  std::any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;
//   std::any visitExpAsRParam(SysYParser::ExpAsRParamContext *ctx) override;
//   std::any visitStringAsRParam(SysYParser::StringAsRParamContext *ctx) override;
  std::any visitNumber(SysYParser::NumberContext *ctx) override;
  std::any visitString(SysYParser::StringContext *ctx) override;
};