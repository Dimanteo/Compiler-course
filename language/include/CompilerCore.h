#ifndef COMPILER_CORE_H
#define COMPILER_CORE_H

#include "Frame.h"
#include "IRGenerator.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <vector>

namespace kolang {

// Singletone with compiler facilities
class CompilerCore {
    IRGenerator IRG;
    // Array of function frames. 0 frame is global scope
    std::vector<Frame> frames;
    // Scope stack
    std::vector<Frame *> frame_stack;
    // Mapping of names encountered during parser to their unique id
    std::unordered_map<std::string, id_t> names_pool;

    CompilerCore();
    CompilerCore(const CompilerCore &) = delete;
    CompilerCore &operator=(CompilerCore &) = delete;

    Frame &getCurrentFrame() const;
    Frame &getGLobalFrame() const;

  public:
    static CompilerCore &getCCore() {
        static CompilerCore CC;
        return CC;
    }
    id_t getNewID() {
        static id_t id = 0;
        return ++id;
    }
    // Get IRGenerator
    IRGenerator &getIRG() { return IRG; }
    void handleAssignment(ASTNode id, ASTNode val);
    ASTNode handleVarUse(ASTNode id);
    id_t registerName(const char *name);
    std::string getNameByID(id_t id) const;
};

}; // namespace kolang

#endif