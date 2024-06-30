#include "../frontend/IR.h"
#include <vector>

namespace transform {

class Unroll {

public:
    // ...
    sysy::Module *module;
    sysy::Function *func;
    sysy::BasicBlock *curBB;
    std::vector<sysy::BasicBlock*> parWBBs;
    Unroll(sysy::Module *module): module(module) {};
    
    bool uLoopDetect(sysy::BasicBlock* targetBB);
    void uLoopTransform(sysy::BasicBlock* targetBB);
    void uLoopScan();
}

}