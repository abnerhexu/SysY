#pragma once
#include <cassert>
#include <iostream>
#include <map>
#include <forward_list>
#include <string>
#include <vector>
#include "IR.h"

namespace sysy {

class SymbolTable {
private:
  enum Kind {
    kModule,
    kFunction,
    kBlock
  };

public:
  struct ModuleScope {
    SymbolTable &table;
    ModuleScope(SymbolTable &table) : table(table) {table.enter(kModule); }
    ~ModuleScope() {table.leave(); }
  };

public:
  struct FunctionScope {
    SymbolTable &table;
    FunctionScope(SymbolTable &table) : table(table) {table.enter(kFunction); }
    ~FunctionScope() {table.leave(); }
  };

public:
  struct BlockScope {
    SymbolTable &table;
    BlockScope(SymbolTable &table) : table(table) {table.enter(kBlock); }
    ~BlockScope() {table.leave(); }
  };

private:
  std::forward_list<std::pair<Kind, std::map<std::string, Value* >>> symbols;

public:
  SymbolTable() = default;

public:
  bool isModuleScope() const { return symbols.front().first == kModule;}
  bool isFunctionScope() const { return symbols.front().first == kFunction;}
  bool isBlockScope() const { return symbols.front().first == kBlock;}

public:
  Value *lookup(const std::string &name) const {
    for (auto &scope: this->symbols) {
      auto it = scope.second.find(name);
      if (it != scope.second.end()) {
        return it->second;
      }
    }
    return nullptr;
  }

public:
  auto insert(const std::string &name, Value *value) {
    assert(not symbols.empty());
    return symbols.front().second.emplace(name, value);
  }

private:
  void enter(Kind kind) {
    symbols.emplace_front();
    symbols.front().first = kind;
  }
  void leave() { symbols.pop_front(); }
}; // class SymbolTable

} // namespace sysy