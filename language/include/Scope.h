#ifndef KOLANG_FRAME_H
#define KOLANG_FRAME_H

#include "IRGenerator.h"

#include <iostream>
#include <map>
#include <string>

#include "llvm/IR/BasicBlock.h"

namespace kolang {

class Scope {
    std::map<strid_t, IRValue> locals;
    llvm::BasicBlock *insertion_point;

  public:
    bool contains(strid_t id) const;
    void setOrUpdateVariable(strid_t id);
    void setOrUpdateVariable(strid_t id, IRValue memory);
    IRValue getVariableValue(strid_t id);
    void yieldInsertBlock();
    void applyInsertBlock();
};

}; // namespace kolang

#endif