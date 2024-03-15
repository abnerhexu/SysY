#include "SymbolTable.h"
#include "IR.h"


sysy::Value* sysy::SymbolTable::lookup(const std::string &name) const {
  for (auto &scope: this->symbols) {
    auto it = scope.second.find(name);
    if (it != scope.second.end()) {
      return it->second;
    }
  }
  return nullptr;
}

auto sysy::SymbolTable::insert(const std::string &name, Value *value) {
  assert(not symbols.empty());
  return symbols.front().second.emplace(name, value);
}

void sysy::SymbolTable::enter(sysy::SymbolTable::Kind kind) {
  symbols.emplace_front();
  symbols.front().first = kind;
}