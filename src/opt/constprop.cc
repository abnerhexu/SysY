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
    std::unordered_map<std::string, std::string> valuelabel;
    // First pass: collect and record constant assignments.
    for (auto &inst : bb->CoInst) {
        if (inst.op == "li" && inst.fields.size() == 2) { // Assuming 'li' is the instruction to load immediate constants.
            if (valueMap.find(inst.fields[0]) == valueMap.end()){
                valueMap[inst.fields[0]] = inst.fields[1]; // Store constant value with its register.
                valuelabel[inst.fields[0]] = "1";
            }
            else {
                valuelabel[inst.fields[0]] = "2";
            }
        }  
    }

    // Second pass: substitute known constants.
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &inst : bb->CoInst) {
            if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[0]) != valueMap.end() && valuelabel[inst.fields[0]] == "1") {
                // Replace uses of constants.
                if (inst.op != "li" ) {
                    inst.fields[0] = valueMap[inst.fields[0]];
                    changed = true;
                }
            }
        }
    }
}


} // namespace transform