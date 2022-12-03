#ifndef KOLANG_FRAME_H
#define KOLANG_FRAME_H

#include "IRGenerator.h"

#include <iostream>
#include <string>
#include <unordered_map>

namespace kolang {

class Frame {
    // Map locals name to their id
    std::unordered_map<id_t, ASTNode /* llvm::Value * */> locals_map;

  protected:
    void addVariable(id_t id, ASTNode value);

  public:
    virtual llvm::BasicBlock *getCurrentBB() = 0;
    virtual ASTNode allocVariable(id_t id);
    ASTNode lookup(id_t id) const;
    bool assignVariable(id_t id, ASTNode value);
};

// Global scope
class GlobalFrame : public Frame {
  public:
    GlobalFrame() = default;
    virtual ASTNode allocVariable(id_t id) override;
    virtual llvm::BasicBlock *getCurrentBB() override;
    virtual ~GlobalFrame() = default;
};

// Functions
class FunctionFrame : public Frame {
    llvm::Function *function;
    // ID of basic block is an ID in this array.
    std::vector<llvm::BasicBlock *> blocks;

  public:
    FunctionFrame(llvm::Function *function);
    static std::string getBBname(unsigned id) {
        return "BB" + std::to_string(id);
    }
    // This is now a boilerplate, but it will be useful for definig lambdas and
    // embedded functions
    virtual llvm::BasicBlock *getCurrentBB() override { return blocks.back(); }
    void allocArgs(ASTNode &args);
    virtual ~FunctionFrame() = default;
};

// Scopes, namespaces etc.
class ScopeFrame : public Frame {
    // BB containing given scope
    llvm::BasicBlock *BB;

  public:
    ScopeFrame(llvm::BasicBlock *block) : BB(block) {}
    virtual llvm::BasicBlock *getCurrentBB() override { return BB; }
    virtual ~ScopeFrame() = default;
};

}; // namespace kolang

#endif