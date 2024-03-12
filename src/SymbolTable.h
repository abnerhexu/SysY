#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "IR.h"

namespace sysy {

class Entry {
  public:
  Value *value;
  bool isConst;
  Entry() = default;
  Entry(Value* v): value(v) {
    isConst = !(value->isPointer());
  }
};

class SymbolTable {
  using tableLine = std::map<std::string, Entry>;
  private:
    std::vector<tableLine> table;
  public:
    bool insert(std::string name, Entry entry);
    Entry* query(std::string name);

    void newLine();
    void pop();
    void view() const;
};

class ArrayEntry {
  public:
    Value* base;
};

}