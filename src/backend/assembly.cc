#include "assembly.h"

namespace codegen {
void AssemblyCode::emitModule(std::ostream &os) {
    os << module->descriptionText;
    os << module->globalDataText;
    auto *funcs = module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        this->emitFunction(os, it->second);
    }
}

void AssemblyCode::emitFunction(std::ostream &os, sysy::Function *func) {
    os << func->getName() << ":\n";
    for (auto &inst: func->MetaInst) {
        inst.print(os);
    }
    // auto bbs = func->getBasicBlocks();
    // for (auto &it: bbs) {
    //     this->emitBasicBlock(os, it.get());
    // }
    auto bb = func->getBasicBlocks().begin()->get();
    std::vector<sysy::BasicBlock *> bbs;
    bbs.push_back(bb);
    while(!bbs.empty()){
        auto bbtop = bbs[bbs.size()-1];
        bbs.pop_back();
        this->emitBasicBlock(os, bbtop);
        for (int i = bbtop->getNumSuccessors()-1; i >= 0; i--){
        bbs.push_back(bbtop->getSuccessors()[i]);
        }
    }
}

void AssemblyCode::emitBasicBlock(std::ostream &os, sysy::BasicBlock *bb) {
    os << bb->bbLabel << ":\n";
    for (auto &inst: bb->CoInst) {
        inst.print(os);
    }
    if (bb->CoInst.empty()) {
        os << "  nop\n";
    }
}

}