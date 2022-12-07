#ifndef KOLANG_IR_GENERATOR_H
#define KOLANG_IR_GENERATOR_H

#include "Kolang.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace kolang {

class ASTNode;

using IRValue = llvm::Value *;

class IRGenerator {
  public:
    static constexpr IRValue NIL() { return nullptr; }
    static bool isNIL(const IRValue &node) { return node == nullptr; }

  private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    size_t module_id = 0;
    size_t bblock_id = 0;

    IRGenerator(const IRGenerator &) = delete;
    IRGenerator &operator=(IRGenerator &) = delete;

    std::string genModuleID() {
        return "KolangM_" + std::to_string(module_id++);
    }
    std::string getBBName() { return "BB" + std::to_string(bblock_id++); }

  public:
    IRGenerator();
    void executeAndFreeModule();
    void dump(std::ostream &outs);
    llvm::Module &getModule() const { return *module.get(); }
    // Generates start routine - it will initialize globals and call main()
    void genStartPrologue();
    // Inserts call to main function
    void genStartEpilogue();
    // Traverse AST an generate IR
    void emitAST(ASTNode *entry);
    llvm::BasicBlock *getInsertBlock() { return builder->GetInsertBlock(); }
    void setInsertBlock(llvm::BasicBlock *bb) { builder->SetInsertPoint(bb); }
    void genIf(IRValue condition, ASTNode *true_path);

    IRValue genAdd(IRValue lhs, IRValue rhs);
    IRValue genSub(IRValue lhs, IRValue rhs);
    IRValue genMul(IRValue lhs, IRValue rhs);
    IRValue genDiv(IRValue lhs, IRValue rhs);
    IRValue genLT(IRValue lhs, IRValue rhs);
    IRValue genGT(IRValue lhs, IRValue rhs);
    IRValue genEQ(IRValue lhs, IRValue rhs);
    IRValue genNEQ(IRValue lhs, IRValue rhs);
    IRValue genLEQ(IRValue lhs, IRValue rhs);
    IRValue genGEQ(IRValue lhs, IRValue rhs);
    IRValue genNumber(numb_t val);
    IRValue allocateGlobal(const std::string &name);
    IRValue allocateLocal();
    IRValue genLoad(IRValue ptr);
    void genStore(IRValue ptr, IRValue value);
    void declFunction(const std::string &name,
                      const std::vector<strid_t> &params);
    void declFunction(const std::string &name);
    IRValue genCall(const std::string &name, const std::vector<IRValue> &args);
    IRValue genCall(const std::string &name);
    void genReturn(IRValue val);
    void genReturn();
};

}; // namespace kolang

#endif