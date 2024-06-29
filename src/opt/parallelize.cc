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

void Parallelize::pLoopTransform() {

}

void Parallelize::LoopScan() {
    auto *funcs = this->module->getFunctions();
    sysy::BasicBlock *curBB;
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        for (auto &bb: it->second->getBasicBlocks()) {
            curBB = bb.get();
            if (curBB->getName().find("while.body") != std::string::npos) {
                if (this->pLoopDetect(curBB)) {
                    
                }
            }
        }
    }
}
}