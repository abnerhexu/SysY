#include "peephole.h"
#include "../frontend/IR.h"
namespace Optimization {

void Hole::functionTransform(sysy::Function *func) {
    for (auto &bb: func->getBasicBlocks()) {
        this->curBB = bb.get();
        this->basicblockTransform(bb.get());
    }
}

void Hole::basicblockTransform(sysy::BasicBlock *bb) {
    int curIndex = 0;
    int nextIndex = 1;
    while (curIndex < bb->CoInst.size() && nextIndex < bb->CoInst.size()) {
        sysy::RVInst curInst = bb->CoInst[curIndex];
        sysy::RVInst nextInst = bb->CoInst[nextIndex];
        if (curInst.op == "lw" && nextInst.op == "sw" && curInst.fields[0] == nextInst.fields[0] && curInst.fields[1] == nextInst.fields[1]) {
            nextInst.valid = false;
            nextIndex++;
            continue;
        }
        if (curInst.op == "sw" && nextInst.op == "lw" && curInst.fields[0] == nextInst.fields[0] && curInst.fields[1] == nextInst.fields[1]) {
            curInst.valid = false;
            nextInst.valid = false;
            curIndex += 2;
            nextIndex += 2;
            continue;
        }
    }
}

}