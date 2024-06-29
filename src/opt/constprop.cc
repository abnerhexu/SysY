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
    std::unordered_map<std::string, int> constantMap;
    for (sysy::RVInst &inst : bb->CoInst) {
        if (inst.op == "li" && inst.fields.size() >= 2) { // Assuming "li" is used to load immediate constants.
            constantMap[inst.fields[1]] = std::stoi(inst.fields[0]);
        } else if (isConstantValue(&inst)) {
            continue; // Skip instructions already known to be constants.
        } else {
            // Attempt to replace uses of constants.
            for (size_t i = 1; i < inst.fields.size(); ++i) {
                if (constantMap.find(inst.fields[i]) != constantMap.end()) {
                    replaceConstantUses(&inst, constantMap[inst.fields[i]]);
                    break; // Assume single replacement per instruction for simplicity.
                }
            }
        }
    }
}

bool ConstProp::isConstantValue(sysy::RVInst *inst) {
    // Implement logic to determine if an instruction's output is a known constant.
    // This is a placeholder and should be tailored to your IR structure.
    return false; // Replace with actual implementation.
}

void ConstProp::replaceConstantUses(sysy::RVInst *defInst, int constantValue) {
    // Replace uses of the defined instruction with the constant value.
    // This is a high-level description and requires detailed implementation.
    // Ensure to update uses in subsequent instructions and potentially remove defInst.
}

} // namespace transform