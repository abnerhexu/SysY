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
    auto bbs = func->getBasicBlocks();
    for (auto &it: bbs) {
        this->emitBasicBlock(os, it.get());
    }
}

void AssemblyCode::emitBasicBlock(std::ostream &os, sysy::BasicBlock *bb) {
    os << bb->bbLabel << ":\n";
    for (auto &inst: bb->CoInst) {
        inst.print(os);
    }
}

}