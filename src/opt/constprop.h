#include "../frontend/IR.h"

namespace transform {

class ConstProp {
private:
    sysy::Module *module;

public:
    sysy::BasicBlock *curBB;

    ConstProp(sysy::Module *module): module(module) {};

    void moduleTransform();
    void functionTransform(sysy::Function *func);
    void constantPropagation(sysy::BasicBlock *bb);
    bool isConstantValue(sysy::RVInst *inst);
    void replaceConstantUses(sysy::RVInst *defInst, int constantValue);
}; // class ConstProp

} // namespace transform