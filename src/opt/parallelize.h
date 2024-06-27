#include "../frontend/IR.h"
#include <vector>

namespace transform {

class Parallelize {

public:
    // ...
    sysy::Function *func;
    sysy::BasicBlock *curBB;
    Parallelize(sysy::Function* func): func(func) {};
    
    bool pLoopDetect();
    void pLoopTransform();
}

}