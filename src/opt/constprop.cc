#include "constprop.h"
#include "../frontend/IR.h"
#include <unordered_map>

namespace transform {

void ConstProp::functionTransform(sysy::Function *func) {
    for (auto &bb : func->getBasicBlocks()) {
        this->curBB = bb.get();
        this->constantPropagation(bb.get());
    }
}

void ConstProp::moduleTransform() {
    auto *funcs = this->module->getFunctions();
    for (auto it = funcs->begin(); it != funcs->end(); ++it) {
        this->functionTransform(it->second);
    }
}

void ConstProp::constantPropagation(sysy::BasicBlock *bb) {
    std::unordered_map<std::string, std::string> valueMap;
    for (auto &inst : bb->CoInst) {
        if (inst.op == "li") { // Assuming 'li' is the instruction to load immediate constants.
            valueMap[inst.fields[0]] = inst.fields[1]; // Store constant value with its register.
        }
        else if(inst.op == "add" || inst.op == "mul" || inst.op == "sub" ||  inst.op == "div" ){
            //int len = inst.fields.size();
            for (int i=1;i<=2;i++){
                if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[i]) != valueMap.end()) {
                // Replace uses of constants.
                    inst.fields[i] = valueMap[inst.fields[i]];
                }
            }
            //int temp1 = (int*) valueMap[inst.fields[1]];
            //valueMap[inst.fields[0]] = valueMap[inst.fields[1]] + valueMap[inst.fields[2]];
        }
        else{
           continue;
        }  
    }

}
} // namespace transform