#ifndef COMPILER_CORE_H
#define COMPILER_CORE_H

#include "AST.h"
#include "IRGenerator.h"
#include "Kolang.h"
#include "Scope.h"

#include <memory>
#include <string>
#include <vector>

namespace kolang {

// Singletone with compiler facilities
class CompilerCore {
    IRGenerator IRG;
    std::vector<ASTNode *> node_storage;
    ASTNode *ast_root;
    // Mapping of names encountered during parser to their unique id
    std::vector<std::string> names_pool;
    // Namespaces stack for tracking available IDs.
    std::vector<Scope> scope_stack;

    CompilerCore() = default;
    CompilerCore(const CompilerCore &) = delete;
    CompilerCore &operator=(CompilerCore &) = delete;

    template <class T, class... Args> T *allocateNode(Args &&...args) {
        node_storage.push_back(
            dynamic_cast<ASTNode *>(new T(std::forward<Args>(args)...)));
        return dynamic_cast<T *>(node_storage.back());
    }

  public:
    static CompilerCore &getCCore() {
        static CompilerCore CC;
        return CC;
    }
    ~CompilerCore();

    template <class T, class... Args> T *make(Args &&...args) {
        return allocateNode<T>(std::forward<Args>(args)...);
    }
    IDNode *makeID(const char *text);

    // Get IRGenerator
    IRGenerator &getIRG() { return IRG; }
    // Traverse AST and generate IR
    void generateIR();
    strid_t getOrCreateID(const char *text);
    std::string &getStringByID(strid_t id);
    ASTNode *getRootNode();
    void setRootNode(ASTNode *node);
    void enterScope();
    void leaveScope();
    IRValue lookup(strid_t id);
    bool inGlobalScope();
    // Adds ids to current scope
    void populateAvailableVariables(strid_t id);
    void populateAvailableVariables(const std::vector<strid_t> &id_list);
    // Assigns value to existing id in current scope
    void addIDValueMapping(strid_t id, IRValue value);
};

}; // namespace kolang

#endif