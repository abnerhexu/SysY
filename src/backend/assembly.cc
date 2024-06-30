#include "assembly.h"

namespace codegen {
void AssemblyCode::emitModule(std::ostream &os) {
    os << module->descriptionText;
    os << module->globalDataText;
    auto *funcs = module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        this->emitFunction(os, it->second);
    }
    os << "  .ident	\"SHCC: (Debian 12.2.0-10) 0.0.1\"\n  .section	.note.GNU-stack,\"\",@progbits\n";
}

void AssemblyCode::emitFunction(std::ostream &os, sysy::Function *func) {
    os << "  .text\n  .align 1\n  .globl " << func->getName() << "\n";
	os << "  .type " << func->getName() << ", @function\n";
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
    os << "  .size " << func->getName() << ", .-" << func->getName() << std::endl;

}

void AssemblyCode::emitBasicBlock(std::ostream &os, sysy::BasicBlock *bb) {
    if (bb->upleveled) {
        os << "  .text\n  .align 1\n  .globl " << bb->getName() << "\n";
	    os << "  .type " << bb->getName() << ", @function\n";
        os << bb->getName() << ":\n";
        os << bb->bbLabel << ":\n";
        for (auto &inst: bb->CoInst) {
            if (inst.op == "ret") {
                for (auto &pinst: bb->getParent()->PostInst) {
                    pinst.print(os);
                }
            }
            inst.print(os);
        }
        if (bb->CoInst.empty()) {
            os << "  nop\n";
        }
        os << "  .size " << bb->getName() << ", .-" << bb->getName() << std::endl;
        return;
    }
    os << bb->bbLabel << ":\n";
    for (auto &inst: bb->CoInst) {
        if (inst.op == "ret") {
            for (auto &pinst: bb->getParent()->PostInst) {
                pinst.print(os);
            }
        }
        inst.print(os);
    }
    if (bb->CoInst.empty()) {
        os << "  nop\n";
    }
}

}