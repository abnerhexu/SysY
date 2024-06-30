#include "peephole.h"
#include "../frontend/IR.h"
namespace transform {

bool isNumber(const std::string& str) {
  return str.find_first_not_of("0123456789") == std::string::npos;
}

void Hole::functionTransform(sysy::Function *func) {
    for (auto &bb: func->getBasicBlocks()) {
        this->curBB = bb.get();
        this->basicblockTransform(bb.get());
        this->basicblockTransform3(bb.get());
        // this->basicblockTransform4(bb.get());
    }
}

void Hole::basicblockTransform(sysy::BasicBlock *bb) {
    if (bb->CoInst.size() <= 2) { return; }
    std::string field1, field2, field3;
    auto curInst = bb->CoInst.begin();
    auto nextInst = std::next(curInst);
    while (curInst != bb->CoInst.end() && nextInst != bb->CoInst.end()) {
        // UB warning
        if (curInst->op == "lw" && nextInst->op == "sw" && curInst->fields[0] == nextInst->fields[0] && curInst->fields[1] == nextInst->fields[1]) {
            bb->CoInst.erase(curInst);
            bb->CoInst.erase(curInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "sw" && nextInst->op == "lw" && curInst->fields[0] == nextInst->fields[0] && curInst->fields[1] == nextInst->fields[1]) {
            bb->CoInst.erase(curInst);
            bb->CoInst.erase(curInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "sw" && nextInst->op == "lw" && curInst->fields[1] == nextInst->fields[1]) {
            bb->CoInst.insert(curInst, sysy::RVInst("mv", nextInst->fields[0], curInst->fields[0]));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "mv" && nextInst->op == "mv" && curInst->fields[0] == nextInst->fields[1]) {
            bb->CoInst.insert(curInst, sysy::RVInst("mv", nextInst->fields[0], curInst->fields[1]));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "li" && nextInst->op == "mv" && curInst->fields[0] == nextInst->fields[1]) {
            bb->CoInst.insert(curInst, sysy::RVInst("li", nextInst->fields[0], curInst->fields[1]));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "li" && nextInst->op == "addi" && nextInst->fields[1] == curInst->fields[0]) {
            auto imm = std::to_string(std::stoi(curInst->fields[1]) + std::stoi(nextInst->fields[2]));
            bb->CoInst.insert(curInst, sysy::RVInst("li", nextInst->fields[0], imm));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "li" && nextInst->op == "add" && nextInst->fields[1] == curInst->fields[0]) {
            auto imm = std::to_string(std::stoi(curInst->fields[1]));
            bb->CoInst.insert(curInst, sysy::RVInst("addi", nextInst->fields[0], nextInst->fields[2], imm));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "li" && nextInst->op == "add" && nextInst->fields[2] == curInst->fields[0]) {
            auto imm = std::to_string(std::stoi(curInst->fields[1]));
            bb->CoInst.insert(curInst, sysy::RVInst("addi", nextInst->fields[0], nextInst->fields[1], imm));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "mv" && nextInst->op == "add" && nextInst->fields[1] == curInst->fields[0]) {
            auto imm = std::to_string(std::stoi(curInst->fields[1]));
            bb->CoInst.insert(curInst, sysy::RVInst("add", nextInst->fields[0], nextInst->fields[2], curInst->fields[1]));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "mv" && nextInst->op == "add" && nextInst->fields[2] == curInst->fields[0]) {
            auto imm = std::to_string(std::stoi(curInst->fields[1]));
            bb->CoInst.insert(curInst, sysy::RVInst("add", nextInst->fields[0], nextInst->fields[1], curInst->fields[1]));
            bb->CoInst.erase(nextInst);
            bb->CoInst.erase(nextInst);
            nextInst = std::next(curInst);
            continue;
        }
        else if (curInst->op == "beq" && nextInst->op == "j" && nextInst->fields[0] == curInst->fields[3]) {
            bb->CoInst.erase(curInst);
            nextInst = std::next(curInst);
            continue;
        }
        curInst = nextInst;
        nextInst = std::next(nextInst);
    }
}

void Hole::moduleTransform() {
    auto *funcs = this->module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        this->functionTransform(it->second);
    }
}

void Hole::basicblockTransform3(sysy::BasicBlock *bb) {
    if (bb->CoInst.size() <= 3) { return; }
    std::string field1, field2, field3;
    auto inst1 = bb->CoInst.begin();
    auto inst2 = std::next(inst1);
    auto inst3 = std::next(inst2);
    while (inst1 != bb->CoInst.end() && inst2 != bb->CoInst.end() && inst3 != bb->CoInst.end()) {
        if (inst1->op == "li" && inst2->op == "li" && inst3->op == "add" && inst3->fields[1] == inst1->fields[0] && inst3->fields[2] == inst2->fields[0]) {
            field1 = inst3->fields[0];
            field2 = std::to_string(std::stoi(inst1->fields[1]) + std::stoi(inst2->fields[1]));
            bb->CoInst.insert(inst1, sysy::RVInst("li", field1, field2));
            bb->CoInst.erase(inst2);
            bb->CoInst.erase(inst2);
            bb->CoInst.erase(inst2);
            inst2 = std::next(inst1);
            inst3 = std::next(inst2);
            continue;
        }
        // else if (inst1->op == "sw" && inst2->op == "lw" && inst3->op == "lw" && inst1->fields[0] != inst2->fields[0] && inst1->fields[0] != inst3->fields[0] && inst1->fields[1] != inst2->fields[1] && inst1->fields[1] == inst3->fields[1]) {
        //     // bb->CoInst.insert(inst1, sysy::RVInst("mv", inst3->fields[0], inst1->fields[0]));
        //     // bb->CoInst.erase(inst2);
        //     // bb->CoInst.erase(inst3);
        //     // inst2 = std::next(inst1);
        //     // inst3 = std::next(inst2);
        //     std::swap(inst2, inst3);
        //     continue;
        // }
        else if (inst1->op == "sub" && inst2->op == "sgtz" && inst3->op == "bnez" && inst1->fields[0] == inst2->fields[0] && inst2->fields[0] == inst2->fields[1] && inst3->fields[0] == inst2->fields[0]) {
            bb->CoInst.insert(inst1, sysy::RVInst("bgt", inst1->fields[1], inst1->fields[2], inst3->fields[1]));
            bb->CoInst.erase(inst2);
            bb->CoInst.erase(inst2);
            bb->CoInst.erase(inst2);
            inst2 = std::next(inst1);
            inst3 = std::next(inst2);
            continue;
        }
        if (inst1->op == "sw" && inst2->op == "li" && inst3->op == "sw" && inst1->fields[0] == inst2->fields[0] && inst3->fields[0] == inst2->fields[0]) {
            inst2->fields[0] = "s1";
            inst3->fields[0] = "s1";
            inst1 = inst2;
            inst2 = inst3;
            inst3 = std::next(inst3);
            continue;
        }
        inst1 = inst2;
        inst2 = inst3;
        inst3 = std::next(inst3);
    }
}

/*
  sw t1, 776(sp)
  lw a0, 796(sp)
  lw a1, 776(sp)
  add t1, a0, a1
*/

void Hole::basicblockTransform4(sysy::BasicBlock *bb) {
    if (bb->CoInst.size() <= 4) { return; }
    std::string field1, field2, field3;
    auto inst1 = bb->CoInst.begin();
    auto inst2 = std::next(inst1);
    auto inst3 = std::next(inst2);
    auto inst4 = std::next(inst3);
    while (inst1 != bb->CoInst.end() && inst2 != bb->CoInst.end() && inst3 != bb->CoInst.end() && inst4 != bb->CoInst.end()) {
        if (inst1->op == "sw" && inst2->op == "lw" && inst3->op == "lw" && inst4->op == "add" && 
            inst1->fields[0] == inst4->fields[0] && inst1->fields[1] == inst3->fields[1] && inst1->fields[1] != inst2->fields[1] && 
            inst1->fields[0] != inst2->fields[0] && inst2->fields[0] != inst3->fields[0] && inst2->fields[0] == inst4->fields[1] && 
            inst3->fields[0] == inst4->fields[2]) {
                bb->CoInst.insert(inst1, sysy::RVInst("mv", inst3->fields[0], inst1->fields[0]));
                bb->CoInst.erase(inst2);
                bb->CoInst.erase(inst3);
                inst2 = std::next(inst1);
                inst3 = std::next(inst2);
                inst4 = std::next(inst3);
                continue;
            }
            
        inst1 = inst2;
        inst2 = inst3;
        inst3 = inst4;
        inst4 = std::next(inst4);
    }
}
}