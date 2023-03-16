#pragma once

#include "SysYBaseVisitor.h"

class ASTPrinter : public SysYBaseVisitor {
public:
  antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;
//   antlrcpp::Any visitExpAsRParam(SysYParser::ExpAsRParamContext *ctx) override;
//   antlrcpp::Any visitStringAsRParam(SysYParser::StringAsRParamContext *ctx) override;
  antlrcpp::Any visitNumber(SysYParser::NumberContext *ctx) override;
  antlrcpp::Any visitString(SysYParser::StringContext *ctx) override;
};