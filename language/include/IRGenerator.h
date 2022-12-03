#ifndef KOLANG_IR_GENERATOR_H
#define KOLANG_IR_GENERATOR_H

#include "ASTNode.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <variant>

namespace kolang {

class Frame;

class IRGenerator {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    size_t module_id = 0;

    IRGenerator(const IRGenerator &) = delete;
    IRGenerator &operator=(IRGenerator &) = delete;

    std::string genModuleID() {
        return "Module_" + std::to_string(module_id++);
    }

  public:
    IRGenerator();
    void executeAndFreeModule();
    void dump(std::ostream &outs);
    llvm::Module &getModule() const { return *module.get(); }
    // User program entry point
    static constexpr const char *getEntryFuncName() { return "main"; }
    // STD lib entry point
    static constexpr const char *getStartFuncName() { return "__kolang_start"; }
    void insertIn(Frame *frame);
    ASTNode genAdd(ASTNode lhs, ASTNode rhs);
    ASTNode genSub(ASTNode lhs, ASTNode rhs);
    ASTNode genMul(ASTNode lhs, ASTNode rhs);
    ASTNode genDiv(ASTNode lhs, ASTNode rhs);
    ASTNode genNumber(int val);
    ASTNode genLocalAlloc();
    ASTNode genGlobalAlloc(std::string &name);
    ASTNode genLoad(ASTNode ptr);
    void genStore(ASTNode ptr, ASTNode value);
    ASTNode genRet(ASTNode val);
    ASTNode genRet();
    // Create function and setInsertPoint to it
    void genFunction(const std::string &name, ASTNode args);
    bool finalizeStartFunction();
    ASTNode genCall(std::string &name, ASTNode args);
};

}; // namespace kolang

#endif