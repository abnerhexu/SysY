#include "../frontend/IR.h"
#include <vector>

namespace transform {

class Parallelize {

public:
    // ...
    sysy::Module *module;
    sysy::Function *func;
    sysy::BasicBlock *curBB;
    std::vector<sysy::BasicBlock*> parWBBs;
    Parallelize(sysy::Module *module): module(module) {};
    
    bool pLoopDetect(sysy::BasicBlock* targetBB);
    void pLoopTransform(sysy::BasicBlock* targetBB);
    void LoopScan();
}

}