#include "parallelize.h"

namespace transform {

bool Parallelize::pLoopDetect(sysy::BasicBlock* targetBB) {
bool parallelizable = false;
    if (targetBB == nullptr) {
        // detect no while loop
        return false;
    }
    std::vector<sysy::LoadInst*>loadInst;
    std::vector<sysy::StoreInst*>storeInst;
    for (auto &inst: targetBB->getInstructions()) {
        if (inst->getKind() == sysy::Value::kLoad) {
            loadInst.push_back(dynamic_cast<sysy::LoadInst*>(inst.get()));
        }
        else if (inst->getKind() == sysy::Value::kStore) {
            storeInst.push_back(dynamic_cast<sysy::StoreInst*>(inst.get()));
        }
    }
    bool SameAddr = false;
    for (auto &it1: storeInst) {
        for (auto &it2: loadInst) {
            if (it1->getNumIndices() && it2->getNumIndices() && it1->getPointer() == it2->getPointer()) {
                SameAddr = true;
                // should take care of loop-carried dependencies
                for (auto a1 = it1->getIndices().begin(), a2 = it2->getIndices().begin(); a1 != it1->getIndices().end(); a1++, a2++) {
                    if (a1->getValue() != a2->getValue()) {
                        return false;
                    }
                }
                break;
            }
        }
    }
    if (SameAddr == false) {
        // no write array is the same as the read, can be parallelized
        return true;
    }
    return false;
    
}

void Parallelize::pLoopTransform(sysy::BasicBlock *wbb) {
    sysy::Value *cond;
    std::vector<sysy::Value*> conds;
    sysy::Value *upper;
    std::vector<sysy::Instruction> reConfiguredInst;
    this->func = wbb->getParent();
    
    for (auto &inst: wbb->getInstructions()) {
        if (inst->getKind() == sysy::Value::kCondBr) {
            cond = dynamic_cast<sysy::CondBrInst*>(inst.get())->getCondition();
        }
    }
    if (cond->getKind() == sysy::Value::Kind::kICmpLT) {
        auto cmp = dynamic_cast<sysy::BinaryInst*>(cond);
        upper = cmp->getRhs();
    }
    auto inst1 = wbb->getInstructions().begin();
    auto inst2 = std::next(inst1);
    auto inst3 = std::next(inst2);
    auto entryBB = wbb->getPredecessors()[0];
    this->generator->builder.setPosition(entryBB->end());
    this->generator->builder.createAllocaInst(sysy::Type::getPointerType(sysy::Type::getIntType()), {}, inst1->get()->getOperand(0)->getName()+"1");
    auto vfork = this->func->addBasicBlock("while.vfork");
    auto t1 = this->func->addBasicBlock("create_t1");
    t1->upleveled = true;
    auto t2 = this->func->addBasicBlock("create_t2");
    t2->upleveled = true;
    entryBB->getSuccessors().clear();
    entryBB->getSuccessors().push_back(vfork);
    vfork->getPredecessors().push_back(entryBB);
    this->generator->builder.setPosition(vfork->begin());
    // sysy::SymbolTable::ModuleScope scope(this->generator->symbols);
    // auto vx = this->generator->builder.createCallInst(dynamic_cast<sysy::Function*>(this->generator->symbols.lookup("vfork")), {}, this->generator->symbols.emitDualVarName("vfork"));
    // auto pid0 = this->generator->builder.createAllocaInst(sysy::Type::getPointerType(sysy::Type::getIntType()), {}, inst1->get()->getOperand(0)->getName()+"pid0");
    // auto pid1 = this->generator->builder.createAllocaInst(sysy::Type::getPointerType(sysy::Type::getIntType()), {}, inst1->get()->getOperand(0)->getName()+"pid1");
    auto vx0 = this->generator->builder.createCallInst(new sysy::Function((this->module), sysy::Type::getFunctionType(sysy::Type::getIntType()), "clone"), {t1}, this->generator->symbols.emitDualVarName("clone"));
    // this->generator->builder.createStoreInst(vx0, pid0);
    auto vx1 = this->generator->builder.createCallInst(new sysy::Function((this->module), sysy::Type::getFunctionType(sysy::Type::getIntType()), "clone"), {t2}, this->generator->symbols.emitDualVarName("clone"));
    // this->generator->builder.createStoreInst(vx1, pid1);
    this->generator->builder.createCallInst(new sysy::Function(this->module, sysy::Type::getFunctionType(sysy::Type::getVoidType()), "waitpid"), {vx0}, "waitpid0");
    this->generator->builder.createCallInst(new sysy::Function(this->module, sysy::Type::getFunctionType(sysy::Type::getVoidType()), "waitpid"), {vx1}, "waitpid1");
    // this->generator->builder.createCallInst(new sysy::Function((this->module), sysy::Type::getFunctionType(sysy::Type::getIntType()), "execve"), {});
    // this->generator->builder.createCondBrInst(vx, t1, t2, {}, {});
    vfork->getSuccessors().push_back(t1);
    t1->getPredecessors().push_back(vfork);
    vfork->getSuccessors().push_back(t2);
    t2->getPredecessors().push_back(vfork);
    t1->getSuccessors().push_back(wbb);
    this->generator->builder.setPosition(t1->end());
    this->generator->builder.createUncondBrInst(wbb, {});
    auto curb = wbb;
    auto pcurb = t2;
    auto ncurb = this->func->addBasicBlock(this->generator->emitBlockName("t2copy"));
    this->generator->builder.setPosition(ncurb->begin());
    this->generator->builder.createUncondBrInst(ncurb, {});
    while (curb->getName().find("while.end") == std::string::npos) {
        pcurb->getSuccessors().push_back(ncurb);
        ncurb->getPredecessors().push_back(pcurb);
        this->generator->builder.setPosition(ncurb->begin());
        for (auto &inst: curb->getInstructions()) {
            if (inst->getKind() == sysy::Value::Kind::kLoad && inst->getOperand(0) == inst1->get()->getOperand(0)) {
                this->generator->builder.createLoadInst(inst1->get()->getOperand(0), {}, inst->getName());
            }
            if (inst->getKind() == sysy::Value::Kind::kStore && inst->getOperand(0) == inst1->get()->getOperand(0)) {
                this->generator->builder.createStoreInst(inst->getOperand(0), inst1->get()->getOperand(0), {}, inst->getName());
            }
            else {
                this->generator->builder.block->getInstructions().emplace(this->generator->builder.position, inst.get());
            }
        }
        curb = curb->getSuccessors()[0];
        pcurb = ncurb;
        ncurb = this->func->addBasicBlock(this->generator->emitBlockName("t2copy"));
    }
    auto exitf = new sysy::Function((this->module), sysy::Type::getFunctionType(sysy::Type::getIntType()), "exit");
    this->generator->builder.setPosition(curb->end());
    this->generator->builder.block->getInstructions().erase(std::prev(curb->end()));
    this->generator->builder.createCallInst(exitf, {new sysy::ConstantValue(0, "zero_exit")}, "exitzero");
    this->generator->builder.setPosition(ncurb->begin());
    this->generator->builder.createCallInst(exitf, {new sysy::ConstantValue(0, "zero_exit")}, "exitzero");
}

void Parallelize::LoopScan() {
    auto *funcs = this->module->getFunctions();
    sysy::BasicBlock *curBB;
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        for (auto &bb: it->second->getBasicBlocks()) {
            curBB = bb.get();
            if (curBB->getName().find("while.body") != std::string::npos) {
                if (this->pLoopDetect(curBB)) {
                    this->pLoopTransform(curBB->getPredecessors()[0]);
                    return;
                }
            }
        }
    }
    
}
}