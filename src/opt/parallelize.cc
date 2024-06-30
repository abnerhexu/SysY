#include "parallelize.h"
#include "../frontend/SysYIRGenerator.h"

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
        if (inst->getType()->getKind() == sysy::Value::kLoad) {
            loadInst.push_back(dynamic_cast<sysy::LoadInst*>(inst.get()));
        }
        else if (inst->getType()->getKind() == sysy::Value::kStore) {
            storeInst.push_back(dynamic_cast<sysy::StoreInst*>(inst.get()));
        }
    }
    bool SameAddr = false;
    for (auto &it1: storeInst) {
        for (auto &it2: loadInst) {
            if (it1->getPointer() == it2->getPointer()) {
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
    
}

void Parallelize::pLoopTransform(sysy::BasicBlock *wbb) {
    sysy::Value *cond;
    std::vector<sysy::Value*> conds;
    sysy::Value *upper;
    std::vector<sysy::Instruction> reConfiguredInst;
    
    for (auto &inst: wbb->getInstructions()) {
        if (inst->getType()->getKind() == sysy::Value::kCondBr) {
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
    this->generator->builder.createAllocaInst(upper->getType(), {}, inst1->get()->getOperand(0)->getName()+"1");
    auto vfork = this->func->addBasicBlock("while.vfork");
    auto t1 = this->func->addBasicBlock("create_t1");
    auto t2 = this->func->addBasicBlock("create_t2");
    entryBB->getSuccessors().clear();
    entryBB->getSuccessors().push_back(vfork);
    vfork->getPredecessors().push_back(entryBB);
    this->generator->builder.setPosition(vfork->begin());
    auto vx = this->generator->builder.createCallInst(dynamic_cast<sysy::Function*>(this->generator->symbols.lookup("vfork")), {}, this->generator->symbols.emitDualVarName("vfork"));
    this->generator->builder.createCondBrInst(vx, t1, t2, {}, {});
    vfork->getSuccessors().push_back(t1);
    t1->getPredecessors().push_back(vfork);
    t1->getSuccessors().push_back(t2);
    t2->getPredecessors().push_back(t1);
    t1->getSuccessors().push_back(wbb->getSuccessors()[0]);
    auto curb = wbb->getSuccessors()[0];
    while (curb->getName().find("while.end") == std::string::npos) {
        auto ncurb = this->func->addBasicBlock(this->generator->emitBlockName("t2copy"));
        this->generator->builder.setPosition(ncurb->begin());
        for (auto &inst: curb->getInstructions()) {
            this->generator
        }
    }
}

// void Parallelize::LoopScan() {
//     auto *funcs = this->module->getFunctions();
//     sysy::BasicBlock *curBB;
//     for (auto it = funcs->begin(); it != funcs->end(); it++) {
//         for (auto &bb: it->second->getBasicBlocks()) {
//             curBB = bb.get();
//             if (curBB->getName().find("while.body") != std::string::npos) {
//                 if (this->pLoopDetect(curBB)) {
//                     sysy::BasicBlock *target = nullptr;
//                     sysy::BasicBlock *trace = curBB;
//                     while (trace->getName().find("while") != std::string::npos) {
//                         for (auto itt: trace->getPredecessors()) {
//                             if (itt->getName().find("while.cond") != std::string::npos) {
//                                 target = itt;
//                                 break;
//                             }
//                         }
//                         trace = trace->getPredecessors()[0];
//                     }
//                     if (target != nullptr) {
//                         this->parWBBs.push_back(target);
//                     }
//                 }
//             }
//         }
//     }
//     for (auto &it: this->parWBBs) {
//         this->pLoopTransform(it);
//     }
// }

void Parallelize::LoopScan() {
    
}
}