#include "unroll.h"

namespace transform {

bool Unroll::uLoopDetect(sysy::BasicBlock* targetBB) {
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

void Unroll::uLoopTransform(sysy::BasicBlock *wbb) {
    auto cond = wbb->getPredecessors()[0];
    // std::cout << cond->CoInst.size() << std::endl;
    if (cond->CoInst.size() != 4) return; //Just support cond such as i < 1024
    else if (wbb->getNumSuccessors() > 1) return; //No multi loop
    else{
        int edge;
        std::string control_var;
        for (auto CurInst = cond->CoInst.begin(); CurInst != cond->CoInst.end(); CurInst = std::next(CurInst)){
            if (CurInst->op == "lw"){
                control_var = CurInst->fields[1];
                break;
            }
        }
        for (auto CurInst = cond->CoInst.begin(); CurInst != cond->CoInst.end(); CurInst = std::next(CurInst)){
            if (CurInst->op == "li"){
                edge = stoi(CurInst->fields[1]);
                break;
            }
        }
        int add_num = 0;
        for (auto CurInst = wbb->CoInst.begin(); CurInst != wbb->CoInst.end(); CurInst = std::next(CurInst)){
            auto NextInst = std::next(CurInst);
            if (CurInst->op == "lw" && CurInst->fields[1] == control_var){
                if (NextInst->op == "addi" && NextInst->fields[1] == CurInst->fields[0]){
                    add_num = stoi(NextInst->fields[2]);
                }
            }
        }
        int unroll_times = 1;
        if (add_num == 0) assert(0);
        else{
            if (edge % add_num) return;
            else edge /= add_num;
        }
        for (int i = 2; i < 10; i++){
            if (edge % i == 0) unroll_times = i;
        }
        // std::cout << unroll_times << std::endl;
        auto j_inst = std::prev(wbb->CoInst.end());
        int unroll_size = wbb->CoInst.size()-1;
        for (int i = 1; i < unroll_times; i++){
            for (int j = 0; j < unroll_size; j++){
                auto CurInst = wbb->CoInst[j];
                auto insert_pos = std::prev(wbb->CoInst.end());
                int inst_size = CurInst.fields.size();
                if (inst_size == 1){
                    wbb->CoInst.insert(insert_pos, sysy::RVInst(CurInst.op, CurInst.fields[0]));
                }else if (inst_size == 2){
                    wbb->CoInst.insert(insert_pos, sysy::RVInst(CurInst.op, CurInst.fields[0], CurInst.fields[1]));
                }else if (inst_size == 3){
                    wbb->CoInst.insert(insert_pos, sysy::RVInst(CurInst.op, CurInst.fields[0], CurInst.fields[1], CurInst.fields[2]));
                }else if (inst_size == 4){
                    wbb->CoInst.insert(insert_pos, sysy::RVInst(CurInst.op, CurInst.fields[0], CurInst.fields[1], CurInst.fields[2], CurInst.fields[3]));
                }else{
                    assert(0);
                }
            }
        }
    }
}

void Unroll::uLoopScan() {
    auto *funcs = this->module->getFunctions();
    sysy::BasicBlock *curBB;
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        for (auto &bb: it->second->getBasicBlocks()) {
            curBB = bb.get();
            if (curBB->getName().find("while.body") != std::string::npos) {
                // if (this->uLoopDetect(curBB)) {
                //     sysy::BasicBlock *target = nullptr;
                //     sysy::BasicBlock *trace = curBB;
                //     while (trace->getName().find("while") != std::string::npos) {
                //         for (auto itt: trace->getPredecessors()) {
                //             if (itt->getName().find("while.cond") != std::string::npos) {
                //                 target = itt;
                //                 break;
                //             }
                //         }
                //         trace = trace->getPredecessors()[0];
                //     }
                //     if (target != nullptr) {
                //         this->parWBBs.push_back(target);
                //     }
                // }
                if (this->uLoopDetect(curBB)) {
                    this->parWBBs.push_back(curBB);
                }
            }
        }
    }
    for (auto &it: this->parWBBs) {
        this->uLoopTransform(it);
    }
}

// void Unroll::uLoopScan() {
    
// }
}