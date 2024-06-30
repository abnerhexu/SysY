#include "../frontend/IR.h"
#include <vector>

namespace transform {

class Parallelize {

public:
    // ...
    sysy::SysYIRGenerator *generator;
    sysy::Module *module;
    sysy::Function *func;
    sysy::BasicBlock *curBB;
    std::vector<sysy::BasicBlock*> parWBBs;
    Parallelize(sysy::Module *module, sysy::SysYIRGenerator *generator): module(module), generator(generator) {};
    
    bool pLoopDetect(sysy::BasicBlock* targetBB);
    void pLoopTransform(sysy::BasicBlock* targetBB);
    void LoopScan();
};

}