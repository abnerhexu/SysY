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
  std::map<Value*, std::string> vNameMap; // alias
  std::map<std::string, int> dualVarNames;

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

  auto insert(const std::string &name, Value *value, const std::string alias = "") {
    assert(not symbols.empty());
    if (alias != "") {
      this->vNameMap.insert({value, alias});
    }
    else {
      this->vNameMap.insert({value, name});
    }
    return symbols.front().second.emplace(name, value);
  }

  std::string emitDualVarName(const std::string &hint = "") {
    if (dualVarNames.find(hint) == dualVarNames.end()) {
      dualVarNames[hint] = 0;
    }
    else {
      dualVarNames[hint]++;
    }
    return hint + std::to_string(dualVarNames[hint]);
  }

  std::string getAlias(Value* value) {
    auto alias = this->vNameMap[value];
    // std::cout << alias << std::endl;
    return alias;
  }

private:
  void enter(Kind kind) {
    symbols.emplace_front();
    symbols.front().first = kind;
  }
  void leave() { symbols.pop_front(); }
}; // class SymbolTable

} // namespace sysy