#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

// Lex property
#define YYSTYPE codak::Node

namespace codak {

using Node = llvm::Value*;

// Singletone with compiler facilities
class IRGenerator {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    size_t module_id = 0;

    IRGenerator();
    IRGenerator(const IRGenerator &) = delete;
    IRGenerator &operator=(IRGenerator &) = delete;

    std::string genModuleID() {
      return "Module_" + std::to_string(module_id++);
    }

  public:
    static IRGenerator &getGenerator() {
        static IRGenerator irg;
        return irg;
    }

    void executeAndFreeModule();
    void dump(std::ostream &outs);

    Node genAdd(Node lhs, Node rhs);
    Node genSub(Node lhs, Node rhs);
    Node genMul(Node lhs, Node rhs);
    Node genDiv(Node lhs, Node rhs);
    Node genNumber(int val);
    Node genRet(Node val);
    Node genRet();
};

}; // namespace codak

#endif