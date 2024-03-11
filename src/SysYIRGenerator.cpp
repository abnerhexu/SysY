#include <iostream>
#include <memory>
#include <vector>
#include "SysYIRGenerator.h"

namespace sysy {

std::any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx) {
  auto pModule = new Module();
  assert(pModule);
  module.reset(pModule);
  visitChildren(ctx);
  return pModule;
}

std::any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx) {
  auto name = ctx->ID()->getText();
  auto params = ctx->funcFParams()->funcFParam();
  std::vector<Type *> paramTypes;
  std::vector<std::string> paramNames;
  for (auto param : params) {
    paramTypes.push_back(std::any_cast<Type *>(visitBtype(param->btype())));
    paramNames.push_back(param->ID()->getText());
  }
  Type *returnType = std::any_cast<Type *>(visitFuncType(ctx->funcType()));
  auto funcType = Type::getFunctionType(returnType, paramTypes);
  auto function = module->createFunction(name, funcType);
  auto entry = function->getEntryBlock();
  for (auto i = 0; i < paramTypes.size(); ++i)
    entry->createArgument(paramTypes[i], paramNames[i]);
  builder.setPosition(entry, entry->end());
  visitBlockStmt(ctx->blockStmt());
  return function;
}
std::any SysYIRGenerator::visitBtype(SysYParser::BtypeContext *ctx) {
  return ctx->INT() ? Type::getIntType() : Type::getFloatType();
}

std::any SysYIRGenerator::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
  for (auto item : ctx->blockItem())
    visitBlockItem(item);
  return builder.getBasicBlock();
}

std::any SysYIRGenerator::visitBlockItem(SysYParser::BlockItemContext *ctx) {
  return ctx->decl() ? visitDecl(ctx->decl()) : visitStmt(ctx->stmt());
}

std::any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx) {
  std::vector<Value *> values;
  auto type = std::any_cast<Type *>(visitBtype(ctx->btype()));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    auto alloca = builder.createAllocaInst(type, {}, name);
    if (varDef->ASSIGN()) {
      auto value = std::any_cast<Value *>(varDef->initValue()->accept(this));
      auto store = builder.createStoreInst(value, alloca);
    }
    values.push_back(alloca);
  }
  return values;
}

} // namespace sysy