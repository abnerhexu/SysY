#include "../frontend/IR.h"
namespace codegen {

class AssemblyCode {
public:
    sysy::Module *module;
    AssemblyCode(sysy::Module* module): module(module) {};

    void emitModule(std::ostream& os);
    void emitFunction(std::ostream& os, sysy::Function* func);
    void emitBasicBlock(std::ostream& os, sysy::BasicBlock* block);

};  // class AssemblyCode
}