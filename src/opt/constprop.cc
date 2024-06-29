#include "constprop.h"
#include "../frontend/IR.h"
#include <unordered_map>
#include <string>

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
    int flag;
    std::unordered_map<std::string, std::string> valueMap;
    std::unordered_map<std::string, std::string> valuechange;
    for (auto &inst : bb->CoInst) {
        if (inst.op == "li") { // Assuming 'li' is the instruction to load immediate constants.
            valueMap[inst.fields[0]] = inst.fields[1]; // Store constant value with its register.
            valuechange[inst.fields[0]] = "0";
        }
        else if(inst.op == "add" ){
            for (int i=1;i<=2;i++){
                flag = false;
                if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[i]) != valueMap.end() && valuechange[inst.fields[i]] == "0") {
                    inst.fields[i] = valueMap[inst.fields[i]];
                    flag = true;
                }
                else break;
            }
            if (flag){
                int temp1 =  std::stoll(inst.fields[1]);
                int temp2 =  std::stoll(inst.fields[2]);
                valueMap[inst.fields[0]] = std::to_string(temp1 + temp2);
            }
            else{
                valuechange[inst.fields[0]] = "1";
            }
        }
        else if(inst.op == "sub" ){
            for (int i=1;i<=2;i++){
                flag = false;
                if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[i]) != valueMap.end() && valuechange[inst.fields[i]] == "0") {
                    inst.fields[i] = valueMap[inst.fields[i]];
                    flag = true;
                }
                else break;
            }
            if (flag){
                int temp1 =  std::stoll(inst.fields[1]);
                int temp2 =  std::stoll(inst.fields[2]);
                valueMap[inst.fields[0]] = std::to_string(temp1 - temp2);
            }
            else{
                valuechange[inst.fields[0]] = "1";
            }
        }
        else if(inst.op == "mul" ){
            for (int i=1;i<=2;i++){
                flag = false;
                if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[i]) != valueMap.end() && valuechange[inst.fields[i]] == "0") {
                    inst.fields[i] = valueMap[inst.fields[i]];
                    flag = true;
                }
                else break;
            }
            if (flag){
                int temp1 =  std::stoll(inst.fields[1]);
                int temp2 =  std::stoll(inst.fields[2]);
                valueMap[inst.fields[0]] = std::to_string(temp1 * temp2);
            }
            else{
                valuechange[inst.fields[0]] = "1";
            }
        }
        else if(inst.op == "div" ){
            for (int i=1;i<=2;i++){
                flag = false;
                if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[i]) != valueMap.end() && valuechange[inst.fields[i]] == "0") {
                    inst.fields[i] = valueMap[inst.fields[i]];
                    flag = true;
                }
                else break;
            }
            if (flag){
                int temp1 =  std::stoll(inst.fields[1]);
                int temp2 =  std::stoll(inst.fields[2]);
                valueMap[inst.fields[0]] = std::to_string(temp1 / temp2);
            }
            else{
                valuechange[inst.fields[0]] = "1";
            }
        }
        else if (inst.op == "sw" ){ 
            auto nextInst = std::next(&inst);
            if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[0]) != valueMap.end() && valuechange[inst.fields[0]] == "0") {
                // Replace uses of constants.
                    inst.fields[0] = valueMap[inst.fields[0]];
            }
            if (nextInst->op == "lw"){
                valueMap[nextInst->fields[0]] = inst.fields[0];// Store constant value with its register.
                valuechange[nextInst->fields[0]] = "0";
            }
        }
        else if (inst.op == "lw" ){ 
            if (inst.valid && !inst.fields.empty() && valueMap.find(inst.fields[0]) != valueMap.end() && valuechange[inst.fields[0]] == "0") {
                // Replace uses of constants.
                    inst.fields[0] = valueMap[inst.fields[0]];
            }
        }
        else{
           continue;
        }  
    }

}
} // namespace transform