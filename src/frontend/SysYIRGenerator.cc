#include "IR.h"
#include <iostream>
#include <memory>
#include <vector>
#include <any>
#include "SysYIRGenerator.h"

namespace sysy {

std::any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx) {
  SymbolTable::ModuleScope scope(symbols);
  auto pModule = new Module();
  assert(pModule);
  module.reset(pModule);
  visitChildren(ctx);
  return pModule;
}

std::any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx) {
  // global and local declarations are handled in different ways
  return symbols.isModuleScope() ? visitGlobalDecl(ctx) : visitLocalDecl(ctx);
}

std::any SysYIRGenerator::visitGlobalDecl(SysYParser::DeclContext *ctx) {
  std::cout << "Do not support!" << std::endl;
  assert(false);
  std::vector<Value *> values;
  bool isConst = ctx->CONST();
  auto type = std::any_cast<Type *>(visitBtype(ctx->btype()));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    std::vector<Value *> dims;
    for (auto exp : varDef->lValue()->exp())
      dims.push_back(std::any_cast<Value *>(exp->accept(this)));
    if (varDef->ASSIGN()) {
      auto p = dynamic_cast<SysYParser::ScalarInitValueContext *>(varDef->initValue());
      if (p->exp()) {
        visitChildren(p->exp());
      }
      auto init = std::any_cast<Value *>(visitScalarInitValue(p));
      values.push_back(module->createGlobalValue(name, type->getPointerType(type), dims, init));
    }
    else {
      values.push_back(module->createGlobalValue(name, type->getPointerType(type), dims, nullptr));
    }
    // auto init = varDef->ASSIGN()
    //                 ? std::any_cast<Value *>(visitScalarInitValue(dynamic_cast<SysYParser::ScalarInitValueContext *>(varDef->initValue())))
    //                 : nullptr;
    // values.push_back(module->createGlobalValue(name, type, dims, init));
  }
  return values;
}

std::any SysYIRGenerator::visitLocalDecl(SysYParser::DeclContext *ctx) {
  std::vector<Value *> values;
  auto type = Type::getPointerType(std::any_cast<Type *>(visitBtype(ctx->btype())));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    auto alloca = builder.createAllocaInst(type, {}, name);
    symbols.insert(name, alloca);
    if (varDef->ASSIGN()) {
      auto p = dynamic_cast<SysYParser::ScalarInitValueContext *>(varDef->initValue());
      if (p->exp()) {
        visitChildren(p->exp());
      }
      auto value = std::any_cast<Value *>(visitScalarInitValue(p));
      // value->setKind(Value::Kind::kConstant); //TODO possible bug
      auto store = builder.createStoreInst(value, alloca);
      // std::cout << "any casted value kind: " << value->getKind() << std::endl;
    }
    values.push_back(alloca);
  }
  return values;
}

std::any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx) {
  // obtain function name and type signature
  auto name = ctx->ID()->getText();
  std::vector<Type *> paramTypes;
  std::vector<std::string> paramNames;
  if (ctx->funcFParams()) {
    auto params = ctx->funcFParams()->funcFParam();
    for (auto param : params) {
      paramTypes.push_back(std::any_cast<Type *>(visitBtype(param->btype())));
      paramNames.push_back(param->ID()->getText());
    }
  }
  Type *returnType = std::any_cast<Type *>(visitFuncType(ctx->funcType()));
  auto funcType = Type::getFunctionType(returnType, paramTypes);
  auto function = module->createFunction(name, funcType);
  symbols.insert(name, function);
  SymbolTable::FunctionScope scope(symbols);
  auto entry = function->getEntryBlock();
  for (auto i = 0; i < paramTypes.size(); ++i) {
    auto arg = entry->createArgument(paramTypes[i], paramNames[i]);
    symbols.insert(paramNames[i], arg);
  }
  builder.setPosition(entry, entry->end());
  visitBlockStmt(ctx->blockStmt());
  return function;
}

std::any SysYIRGenerator::visitBtype(SysYParser::BtypeContext *ctx) {
  return ctx->INT() ? Type::getIntType() : Type::getFloatType();
}

std::any SysYIRGenerator::visitFuncType(SysYParser::FuncTypeContext *ctx) {
  return ctx->INT()
             ? Type::getIntType()
             : (ctx->FLOAT() ? Type::getFloatType() : Type::getVoidType());
}

std::any SysYIRGenerator::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
  for (auto item : ctx->blockItem()) {
    // if (1) {
    //   std::cout << (item->decl()? "decl": "no decl") << (item->stmt()? "stmt": "no stmt") << std::endl;
    // }
    visitBlockItem(item);
  }
  return builder.getBasicBlock();
}

std::any SysYIRGenerator::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
  auto rhs = std::any_cast<Value *>(ctx->exp()->accept(this));
  auto lValue = ctx->lValue();
  auto name = lValue->ID()->getText();
  auto pointer = symbols.lookup(name);
  Value *store = builder.createStoreInst(rhs, pointer);
  return store;
}

std::any SysYIRGenerator::visitNumberExp(SysYParser::NumberExpContext *ctx) {
  Value *result = nullptr;
  assert(ctx->number()->ILITERAL() or ctx->number()->FLITERAL());
  if (auto iLiteral = ctx->number()->ILITERAL())
    result = ConstantValue::get(std::stoi(iLiteral->getText()));
  else
    result =
        ConstantValue::get(std::stof(ctx->number()->FLITERAL()->getText()));
  return result;
}

std::any SysYIRGenerator::visitLValueExp(SysYParser::LValueExpContext *ctx) {
  auto name = ctx->lValue()->ID()->getText();
  Value *value = symbols.lookup(name);
  if (isa<GlobalValue>(value) or isa<AllocaInst>(value))
    value = builder.createLoadInst(value);
  return value;
}

std::any SysYIRGenerator::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) {
  auto lhs = std::any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = std::any_cast<Value *>(ctx->exp(1)->accept(this));
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  Value *result = nullptr;
  if (ctx->ADD())
    result = type->isInt() ? builder.createAddInst(lhs, rhs)
                           : builder.createFAddInst(lhs, rhs);
  else
    result = type->isInt() ? builder.createSubInst(lhs, rhs)
                           : builder.createFSubInst(lhs, rhs);
  return result;
}

std::any SysYIRGenerator::visitEqualExp(SysYParser::EqualExpContext *ctx) {
  auto lhs = std::any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = std::any_cast<Value *>(ctx->exp(1)->accept(this));
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  Value *result = nullptr;
  if (ctx->EQ())
    result = type->isInt() ? builder.createICmpEQInst(lhs, rhs)
                           : builder.createFCmpEQInst(lhs, rhs);
  else
    result = type->isInt() ? builder.createICmpNEInst(lhs, rhs)
                           : builder.createFCmpNEInst(lhs, rhs);
  return result;
}

std::any SysYIRGenerator::visitOrExp(SysYParser::OrExpContext *ctx) {
  auto lhs = std::any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = std::any_cast<Value *>(ctx->exp(1)->accept(this));
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  Value *result = nullptr;
  if (ctx->OR())
    result = type->isInt() ? builder.createOrInst(lhs, rhs)
                           : builder.createFOrInst(lhs, rhs);
  return result;
}

std::any SysYIRGenerator::visitUnaryExp(SysYParser::UnaryExpContext *ctx) {
  auto v = std::any_cast<Value *>(ctx->exp()->accept(this));
  auto type = v->getType();
  Value* result = nullptr;
  if (ctx->SUB()) {
    result = type->isInt() ? builder.createNegInst(v) : builder.createFNegInst(v);
  }
  else if (ctx->NOT()) {
    result = builder.createNotInst(v);
  }
  return result;
}

std::any SysYIRGenerator::visitMultiplicativeExp(
    SysYParser::MultiplicativeExpContext *ctx) {
  auto lhs = std::any_cast<Value *>(ctx->exp(0)->accept(this));
  auto rhs = std::any_cast<Value *>(ctx->exp(1)->accept(this));
  auto lhsTy = lhs->getType();
  auto rhsTy = rhs->getType();
  auto type = getArithmeticResultType(lhsTy, rhsTy);
  if (lhsTy != type)
    lhs = builder.createIToFInst(lhs);
  if (rhsTy != type)
    rhs = builder.createIToFInst(rhs);
  Value *result = nullptr;
  if (ctx->MUL())
    result = type->isInt() ? builder.createMulInst(lhs, rhs)
                           : builder.createFMulInst(lhs, rhs);
  else if (ctx->DIV())
    result = type->isInt() ? builder.createDivInst(lhs, rhs)
                           : builder.createFDivInst(lhs, rhs);

  else
    result = type->isInt() ? builder.createRemInst(lhs, rhs)
                           : builder.createFRemInst(lhs, rhs);
  return result;
}

std::any SysYIRGenerator::visitReturnStmt(SysYParser::ReturnStmtContext *ctx) {
  auto value = ctx->exp() ? std::any_cast<Value *>(ctx->exp()->accept(this)) : nullptr;
  Value *result = builder.createReturnInst(value);
  return result;
}

std::any SysYIRGenerator::visitCall(SysYParser::CallContext *ctx) {
  auto funcName = ctx->ID()->getText();
  auto func = dynamicCast<Function>(symbols.lookup(funcName));
  assert(func);
  std::vector<Value *> args;
  if (auto rArgs = ctx->funcRParams()) {
    for (auto exp : rArgs->exp()) {
      args.push_back(std::any_cast<Value *>(exp->accept(this)));
    }
  }
  Value *call = builder.createCallInst(func, args);
  return call;
}

std::any SysYIRGenerator::visitIfStmt(SysYParser::IfStmtContext *ctx) {
  auto cond = std::any_cast<Value *>(ctx->exp()->accept(this));
  assert(ctx->stmt(0));
  assert(cond);
  auto* curBlock = builder.getBasicBlock();
  auto* func = curBlock->getParent();
  auto* thenBlock = func->addBasicBlock("then");
  auto* elseBlock = func->addBasicBlock("else");
  auto* exitBlock = func->addBasicBlock("exit");
  builder.createCondBrInst(cond, thenBlock, elseBlock, {}, {});
  curBlock->getSuccessors().push_back(thenBlock);
  thenBlock->getPredecessors().push_back(curBlock);
  builder.setPosition(thenBlock, thenBlock->end());
  visitStmt(ctx->stmt(0));
  builder.createUncondBrInst(exitBlock, {});
}

} // namespace sysy