#include "../../frontend/IR.h"
#include "../../frontend/SysYIRGenerator.h"
#include "../../frontend/IR.h"
#include <string>
#include <vector>

namespace arch {
class ArchitectureSchedule {
public:
  sysy::Module *module;
  ArchitectureSchedule(sysy::Module *module): module(module) {};
  void ModuleScheduler();
  void FunctionScheduler(sysy::Function* func);
  void BasicBlockScheduler(sysy::BasicBlock* bb);
};
}