#include "architecture.h"

namespace arch {

struct instNode {
    std::vector<sysy::RVInst*> post;
    std::vector<sysy::RVInst*> pre;
    sysy::RVInst *cur;
    instNode(sysy::RVInst* cur): cur(cur) {post.resize(0); pre.resize(0); }
};

void ArchitectureSchedule::ModuleScheduler() {
    auto *funcs = this->module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        this->FunctionScheduler(it->second);
    }
}

void ArchitectureSchedule::FunctionScheduler(sysy::Function *func) {
    for (auto &bb: func->getBasicBlocks()) {
        if (bb->getName().find("while.body") != std::string::npos && bb->getInstructions().size() > 20) {
            this->BasicBlockScheduler(bb.get());
        }
        // this->basicblockTransform4(bb.get());
    }
}

void ArchitectureSchedule::BasicBlockScheduler(sysy::BasicBlock* bb) {
    if (bb->getInstructions().size() < 4) {
        return;
    }
    std::vector<instNode*> info;
    for (auto inst = bb->CoInst.begin(); inst != bb->CoInst.end(); inst++) {
        if (inst->op == "j") { if (std::next(inst) == bb->CoInst.end()) { break; } else { continue; } }
        auto t = new instNode(&*inst);
        info.push_back(t);
        if (&*inst == &bb->CoInst.front()) { continue; }
        for (auto ppre = std::prev(inst); ppre != bb->CoInst.begin(); ppre--) {
            for (auto field: inst->fields) {
                for (auto ppf: ppre->fields) {
                    if (field == ppf) {
                        t->pre.insert(t->pre.begin(), &*ppre);
                        break;
                    }
                }
            }
        }
    }
    std::vector<instNode*> roots;
    std::vector<sysy::RVInst> scheduled;
    auto inst = info.begin();
    while (inst != info.end()) {
        if ((*inst)->pre.size() == 0) {
            roots.push_back(*inst);
            info.erase(inst);
        }
        else {
            inst++;
        }
    }
    while (roots.size() > 0) {
        auto t = roots.begin();
        for (auto ninst: info) {
            auto ntf = ninst->pre.begin();
            while (ntf != ninst->pre.end()) {
                if (*ntf == (*t)->cur) {
                    ninst->pre.erase(ntf);
                }
                else { ntf++; }
            }
        }
        scheduled.push_back(*(*t)->cur);
        roots.erase(t);
        auto tinst = info.begin(); {
            while (tinst != info.end()) {
                if ((*tinst)->pre.size() == 0) {
                    roots.push_back(*tinst);
                    info.erase(tinst);
                }
                else {
                    tinst++;
                }
            }
        }
    }
    if (bb->CoInst.back().op == "j") {
        scheduled.push_back(bb->CoInst.back());
    }
    bb->CoInst = scheduled;
}

}