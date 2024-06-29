#include "IR.h"
#include <iostream>
#include <memory>
#include <vector>
#include <any>
#include <string>
#include "SysYIRGenerator.h"

namespace sysy {

std::map<std::string, std::vector<Value *>> usedarrays;

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
  // std::cout << "Do not support!" << std::endl;
  // assert(false);
  std::vector<Value *> values;
  std::vector<Value*> init;
  bool isConst = ctx->CONST();
  auto type = std::any_cast<Type *>(visitBtype(ctx->btype()));
  for (auto varDef : ctx->varDef()) {
    sysy::GlobalValue* GlobalValue;
    auto name = varDef->lValue()->ID()->getText();
    std::vector<Value *> dims;
    for (auto exp : varDef->lValue()->exp()){
      auto target = symbols.lookup(exp->getText());
      // if (target != nullptr)
      //   std::cout << target->getName() << ' ' << (target->getKind() == Value::Kind::kGlobal) << std::endl;
      // else
      //   std::cout << "null" << std::endl;
      if (target != nullptr){
        auto initVal = dynamic_cast<sysy::GlobalValue *>(target)->getInitVals(0);
        dims.push_back(initVal);
      }else
        dims.push_back(std::any_cast<Value *>(exp->accept(this)));
    }
    if (varDef->ASSIGN()) {
      if (varDef->lValue()->exp(0) == 0) {
        auto p = dynamic_cast<SysYParser::ScalarInitValueContext *>(varDef->initValue());
        if (p->exp()) {
          visitChildren(p->exp());
        }
        init.push_back(std::any_cast<Value *>(visitScalarInitValue(p)));
        GlobalValue = module->createGlobalValue(name, type->getPointerType(type), dims, init);
        values.push_back(GlobalValue);
      } // scalar
      else {
        auto p = dynamic_cast<SysYParser::ArrayInitValueContext *>(varDef->initValue());
        auto initVals = std::any_cast<std::vector<Value*>>(visitArrayInitValue(p));
        GlobalValue = module->createGlobalValue(name, type->getPointerType(type), dims, initVals);
        values.push_back(GlobalValue);
        usedarrays.insert({name, dims});
      } // array
    }
    else {
      GlobalValue = module->createGlobalValue(name, type->getPointerType(type), dims, init);
      values.push_back(GlobalValue);
      if (dims.size() > 0) {
        usedarrays.insert({name, dims});
      }
    }
    symbols.insert(name, GlobalValue);
  }
  return values;
}

std::any SysYIRGenerator::visitLocalDecl(SysYParser::DeclContext *ctx) {
  std::vector<Value *> values;
  sysy::AllocaInst *alloca;
  auto type = Type::getPointerType(std::any_cast<Type *>(visitBtype(ctx->btype())));
  for (auto varDef : ctx->varDef()) {
    auto name = varDef->lValue()->ID()->getText();
    auto alias = symbols.emitDualVarName(name);
    // std::cout << name << " 0" << std::endl;
    // auto foundV = symbols.lookup(name);
    // auto name2 = symbols.getAlias(foundV);
    // std::cout << name << " " << name2 << std::endl;
    std::vector<Value *> dims;
    for (auto exp : varDef->lValue()->exp())
      dims.push_back(std::any_cast<Value *>(exp->accept(this)));
    alloca = builder.createAllocaInst(type, dims, alias);
    symbols.insert(name, alloca, alias);
    name = alias;
    if (varDef->lValue()->exp(0) == 0) {
      // scalar
      if (varDef->ASSIGN()) {
        auto p = dynamic_cast<SysYParser::ScalarInitValueContext *>(varDef->initValue());
        auto value = std::any_cast<Value *>(visitScalarInitValue(p));
        auto store = builder.createStoreInst(value, alloca);
      }
    }
    else{
      // array
      usedarrays.insert({name, dims});
      if (varDef->ASSIGN()) {
        std::vector<Value *>indices;
        Value *arrayindex = new ConstantValue(0); // caution: memory leak risk!!!
        indices.push_back(arrayindex);
        auto p = dynamic_cast<SysYParser::ArrayInitValueContext *>(varDef->initValue());
        auto values = std::any_cast<std::vector<Value *>>(visitArrayInitValue(p));
        // auto store = builder.createStoreInst(value, alloca);
        for (int i = 0; i < values.size(); i++){
          arrayindex = new ConstantValue(i); // caution: memory leak risk!!!
          indices[0] = arrayindex;
          builder.createStoreInst(values[i], alloca, indices, "", "true");
        }
      }
    }
    values.push_back(alloca);
  }
  return alloca;
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

std::any SysYIRGenerator::visitScalarInitValue(SysYParser::ScalarInitValueContext *ctx) {
  // Value *value =  std::any_cast<Value *>(ctx->exp()->accept(this));
  // std::cout << (dynamicCast<ConstantValue>(value))->getInt() << std::endl;
  return visitChildren(ctx);
}

std::any SysYIRGenerator::visitArrayInitValue(SysYParser::ArrayInitValueContext *ctx) {
  std::vector<Value *>values;
  // std::cout << ctx->getText() << ' ' << ctx->initValue().size() << std::endl;
  for (int i = 0; i < ctx->initValue().size(); i++){
    // value = std::any_cast<Value *>(visitScalarInitValue(dynamic_cast<SysYParser::ScalarInitValueContext *>(ctx->initValue()[i])));
    values.push_back(std::any_cast<Value *>(ctx->initValue()[i]->accept(this)));
    // std::cout << (dynamicCast<ConstantValue>(value))->getInt() << std::endl;
  }
  return values;
}


std::any SysYIRGenerator::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
  auto rhs = std::any_cast<Value *>(ctx->exp()->accept(this));
  auto lValue = ctx->lValue();
  auto name = lValue->ID()->getText();
  auto pointer = symbols.lookup(name);
  name = symbols.getAlias(pointer);
  auto shape = usedarrays.find(name);
  std::vector<Value *> irs;
  std::vector<Value *> add_results;
  // std::cout << pointer << name << std::endl;
  std::vector <Value *> indices;
  // handle store a number into a scalar var
  if (shape == usedarrays.end()) {
    return builder.createStoreInst(rhs, pointer);
  }
  // array
  if (shape->second.size() > 1){
    irs.push_back(shape->second[shape->second.size()-1]);
    for (int i = 1; i < shape->second.size()-1; i++){
      irs.push_back(builder.createMulInst(irs[i-1], shape->second[shape->second.size()-i-1]));
    }
  }
  int j = 0;
  for (auto &exp: ctx->lValue()->exp()) {
    if (irs.empty()) {
      // 1-dimensional array
      indices.push_back(std::any_cast<Value *>(exp->accept(this)));
    }
    else {
      if (j == shape->second.size()-1){
        add_results.push_back(builder.createAddInst(add_results[j-1], std::any_cast<Value *>(exp->accept(this))));
      }else{
        Value * mul_result = builder.createMulInst(std::any_cast<Value*>(exp->accept(this)), irs[shape->second.size()-j-2]);
        if (j == 0)
          add_results.push_back(mul_result);
        else
          add_results.push_back(builder.createAddInst(add_results[j-1], mul_result));
      }
      j++;
    }
  }
  if (!irs.empty())
    indices.push_back(add_results[add_results.size()-1]);
  Value *store;
  //std::cout << name << " " << dynamic_cast<AllocaInst*>(pointer)->getNumDims() << std::endl;
  if (indices.size() > 0){
    store = builder.createStoreInst(rhs, pointer, indices, "", "true"); // stores in an array
    // std::cout << rhs->getName() << std::endl;
    // assert(0);
  }
  else {
    store = builder.createStoreInst(rhs, pointer, indices);
  }
  irs.push_back(store);
  return irs;
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
  // std::cout << name << std::endl;
  Value *value = symbols.lookup(name);
  // std::cout << name << std::endl;
  // std::cout << value->getName() << std::endl;
  assert(value != nullptr);
  name = symbols.getAlias(value);
  // std::cout << name << std::endl;
  auto shape = usedarrays.find(name);

  //scalar
  if (shape == usedarrays.end()){
    // function params
    if (isa<GlobalValue>(value) or isa<AllocaInst>(value)) {
      value = builder.createLoadInst(value);
    }
  }
  //array
  else{
    std::vector <Value *> indices;
    std::vector<Value *> irs;
    std::vector<Value *> add_results;
    if (shape->second.size() > 1){
      irs.push_back(shape->second[shape->second.size()-1]);
      for (int i = 1; i < shape->second.size()-1; i++){
        irs.push_back(builder.createMulInst(irs[i-1], shape->second[shape->second.size()-i-1]));
      }
    }
    int j = 0;
    for (auto exp : ctx->lValue()->exp()){
      if (irs.empty()) {
        // 1-dimensional array
        indices.push_back(std::any_cast<Value *>(exp->accept(this)));
      }
      else {
        if (j == shape->second.size()-1){
          add_results.push_back(builder.createAddInst(add_results[j-1], std::any_cast<Value *>(exp->accept(this))));
        }else{
          Value * mul_result = builder.createMulInst(std::any_cast<Value*>(exp->accept(this)), irs[shape->second.size()-j-2]);
          if (j == 0)
            add_results.push_back(mul_result);
          else
            add_results.push_back(builder.createAddInst(add_results[j-1], mul_result));
        }
        j++;
      }
    }
    if (!irs.empty())
      indices.push_back(add_results[add_results.size()-1]);
    if (isa<GlobalValue>(value) or isa<AllocaInst>(value))
      value = builder.createLoadInst(value, indices);
  }
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

std::any SysYIRGenerator::visitRelationExp(SysYParser::RelationExpContext *ctx) {
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
  if (ctx->LT())
    result = type->isInt() ? builder.createICmpLTInst(lhs, rhs)
                           : builder.createFCmpLTInst(lhs, rhs);
  else if (ctx->GT())
    result = type->isInt() ? builder.createICmpGTInst(lhs, rhs)
                           : builder.createFCmpGTInst(lhs, rhs);
  else if (ctx->LE())
    result = type->isInt() ? builder.createICmpLEInst(lhs, rhs)
                           : builder.createFCmpLEInst(lhs, rhs);
  else if (ctx->GE())
    result = type->isInt() ? builder.createICmpGEInst(lhs, rhs)
                           : builder.createFCmpGEInst(lhs, rhs);
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
  else if (ctx->NE())
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

std::any SysYIRGenerator::visitAndExp(SysYParser::AndExpContext *ctx) {
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
  if (ctx->AND())
    result = type->isInt() ? builder.createAndInst(lhs, rhs)
                           : builder.createFAndInst(lhs, rhs);
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
  else if (ctx->ADD()) {
    result = v;
  }
  return result;
}

std::any SysYIRGenerator::visitParenExp(SysYParser::ParenExpContext *ctx) {
    return std::any_cast<Value *>(ctx->exp()->accept(this));
    // return visitChildren(ctx);
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

std::any SysYIRGenerator::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
  //std::any_cast<Value *>(ctx->parent->accept(this));
  auto* curBlock = builder.getBasicBlock();
  auto* dest = curBlock;
  std::string name = dest->getName();
  int cnt = 1;
  while (cnt > 0){
    if (name.compare(0, 9, "while.end") == 0) cnt++;
    dest = dest->getPredecessors()[0];
    name = dest->getName();
    if (name.compare(0, 10, "while.cond") == 0) cnt--;
  }
  // std::cout << name << std::endl;
  Value* result = builder.createUncondBrInst(dest, {});
  return result;
}

std::any SysYIRGenerator::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
  auto* curBlock = builder.getBasicBlock();
  auto* func = curBlock->getParent();
  auto* dest = curBlock;
  std::string name = dest->getName();
  // std::cout << "cur:" << name << std::endl;
  int cnt = 1;
  while (cnt > 0){
    if (name.compare(0, 9, "while.end") == 0) cnt++;
    dest = dest->getPredecessors()[0];
    name = dest->getName();
    if (name.compare(0, 10, "while.cond") == 0) cnt--;
  }
  // std::cout << "dest:" << name << std::endl;
  // auto* dest_body = dest->getSuccessors()[dest->getNumSuccessors()-1];
  // std::cout << dest_body->getNumSuccessors() << std::endl;
  name = dest->getName();
  auto dest_num = name.substr(10);
  // std::cout << dest_num << std::endl;
  for (auto &bb: func->getBasicBlocks()){
    auto dest_name = bb->getName();
    if (dest_name.compare(0, 9, "while.end") == 0 && dest_name.substr(9) == dest_num){
      dest = bb.get();
      break;
    }
  }
  // for (int i = 0; i < dest_body->getNumSuccessors(); i++){
  //   dest = dest_body->getSuccessors()[i];
  //   name = dest->getName();
  //   // std::cout << name << std::endl;
  //   //std::cout << name << ' ' << dest->getNumSuccessors() << std::endl;
  //   if (name.compare(0, 9, "while.end") == 0)
  //     break;
  // }
  //std::cout << name << std::endl;
  Value* result = builder.createUncondBrInst(dest, {});
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
  // std::cout << ctx->exp()->getText() << std::endl;
  // std::cout << cond->getKind() << std::endl;
  assert(ctx->stmt(0));
  assert(cond);
  auto* curBlock = builder.getBasicBlock();
  auto* func = curBlock->getParent();
  auto allocatedAnyID = AllocateNamedBlockID("if.header.");
  auto* thenBlock = func->addBasicBlock(emitBlockName("if.then.", allocatedAnyID));
  auto* elseBlock = func->addBasicBlock(emitBlockName("if.else.", allocatedAnyID));
  auto* exitBlock = func->addBasicBlock(emitBlockName("if.exit.", allocatedAnyID));
  builder.createCondBrInst(cond, thenBlock, elseBlock, {}, {});
  curBlock->getSuccessors().push_back(thenBlock);
  thenBlock->getPredecessors().push_back(curBlock);
  builder.setPosition(thenBlock, thenBlock->end());
  visitStmt(ctx->stmt(0));
  builder.createUncondBrInst(exitBlock, {});
  // auto* thenExitBlock = builder.getBasicBlock();

  thenBlock->getSuccessors().push_back(elseBlock);
  elseBlock->getPredecessors().push_back(thenBlock);
  builder.setPosition(elseBlock, elseBlock->end());
  if (ctx->ELSE()) {
    visitStmt(ctx->stmt(1));
  }
  builder.createUncondBrInst(exitBlock, {});
  auto* elseExitBlock = builder.getBasicBlock();
  // thenExitBlock->getSuccessors().push_back(exitBlock);
  // exitBlock->getPredecessors().push_back(thenExitBlock);
  elseExitBlock->getSuccessors().push_back(exitBlock);
  exitBlock->getPredecessors().push_back(elseExitBlock);
  builder.setPosition(exitBlock, exitBlock->end());
  return builder.getBasicBlock();
}

std::any SysYIRGenerator::visitWhileStmt(SysYParser::WhileStmtContext *ctx) {
  auto* curBlock = builder.getBasicBlock();
  auto* func = curBlock->getParent();
  auto allocatedAnyID = AllocateNamedBlockID("while.cond.");
  auto* headerBlock = func->addBasicBlock(emitBlockName("while.cond.", allocatedAnyID));
  headerBlock->setKind(BasicBlock::BBKind::kWhileHeader);
  auto* thenBlock = func->addBasicBlock(emitBlockName("while.body.", allocatedAnyID));
  thenBlock->setKind(BasicBlock::BBKind::kWhileBody);
  auto* exitBlock = func->addBasicBlock(emitBlockName("while.end.", allocatedAnyID));
  exitBlock->setKind(BasicBlock::BBKind::kWhileEnd);
  curBlock->getSuccessors().push_back(headerBlock);
  headerBlock->getPredecessors().push_back(curBlock);
  builder.setPosition(headerBlock, headerBlock->end());
  auto cond = std::any_cast<Value *>(ctx->exp()->accept(this));
  builder.createCondBrInst(cond, thenBlock, exitBlock, {}, {});
  headerBlock->getSuccessors().push_back(thenBlock);
  thenBlock->getPredecessors().push_back(headerBlock);

  builder.setPosition(thenBlock, thenBlock->end());
  auto *bb1 = builder.getBasicBlock();
  visitStmt(ctx->stmt());
  builder.createUncondBrInst(headerBlock, {});
  thenBlock->getSuccessors().push_back(exitBlock);
  exitBlock->getPredecessors().push_back(thenBlock);
  auto *bb2 = builder.getBasicBlock();
  builder.setPosition(thenBlock, thenBlock->end());
  // std::cout << thenBlock->getName() << ' ' << thenBlock->getSuccessors()[0]->getName() << std::endl;
  // if (bb1 != bb2) {
  //   // std::cout << thenBlock->getName() << ' ' << bb1->getNumSuccessors() << ' ' << bb1->getSuccessors()[1]->getName() << std::endl;
  //   // builder.createUncondBrInst(bb2->getPredecessors()[0]->getPredecessors()[0], {});
  //   builder.createUncondBrInst(bb1->getSuccessors()[0], {});
  // }
  
  // builder.setPosition(thenBlock, thenBlock->end());
  // builder.createUncondBrInst(thenBlock->getSuccessors()[thenBlock->getSuccessors().size() - 1], {});
  // builder.createUncondBrInst()
  builder.setPosition(exitBlock, exitBlock->end());
  return builder.getBasicBlock();
}
} // namespace sysy