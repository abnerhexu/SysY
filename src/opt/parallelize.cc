#include "parallelize.h"

namespace optimization {

bool Parallelize::pLoopDetect() {
    std::map<std::string, sysy::Function*> *funcs = module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); it++) {
        sysy::Function *func = it->second;
        auto bbs = func->getBasicBlocks();
        if (bbs.empty()) {
        continue;
        }
        for (auto &bb: bbs) {
            if (bb->getKind() == sysy::BasicBlock::BBKind::kWhileHeader) {
                
            }
        }
    }
}

void Parallelize::pLoopTransform() {

}

}