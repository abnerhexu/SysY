#include "../frontend/IR.h"

namespace optimization {

class Parallelize {

public:
    // ...
    sysy::Module *module;
    sysy::Function *curFunc;
    sysy::BasicBlock *curBB;
    Parallelize(sysy::Module* module): module(module) {};
    
    bool pLoopDetect();
    void pLoopTransform();
}

}