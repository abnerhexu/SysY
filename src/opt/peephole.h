#include "../frontend/IR.h"
#include<string>

namespace transform {
class Hole {
private:
  sysy::Module *module;
public:
  sysy::BasicBlock *curBB;
public:
  Hole(sysy::Module *module): module(module) {};
  void moduleTransform();
  void functionTransform(sysy::Function *func);
  void basicblockTransform(sysy::BasicBlock *bb);
  void basicblockTransform3(sysy::BasicBlock *bb);
  void basicblockTransform4(sysy::BasicBlock *bb);
  void loadStoreEliminate(sysy::RVInst *inst1, sysy::RVInst *inst2);
  void storeLoadEliminate(sysy::RVInst *inst1, sysy::RVInst *inst2);
}; // class hole
}