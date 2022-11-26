#ifndef KOLANG_FRAME_H
#define KOLANG_FRAME_H

#include "IRGenerator.h"

#include <string>
#include <unordered_map>

namespace kolang {

// Scope representation. Contains locals and code of this scope
class Frame {
    // Map locals name to their id
    std::unordered_map<id_t, ASTNode /* llvm::Value * */> locals_map;

  protected:
    void addVariable(id_t id, ASTNode value);

  public:
    ASTNode lookup(id_t id) const;
    virtual ASTNode allocVariable(id_t id);
    bool assignVariable(id_t id, ASTNode value);
    virtual ~Frame() = default;
};

class GlobalFrame : public Frame {
  public:
    virtual ASTNode allocVariable(id_t id) override;
    virtual ~GlobalFrame() = default;
};

}; // namespace kolang

#endif