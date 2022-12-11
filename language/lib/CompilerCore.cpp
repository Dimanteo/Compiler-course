#include "CompilerCore.h"
#include "AST.h"

namespace kolang {

CompilerCore::~CompilerCore() {
    for (ASTNode *node : node_storage) {
        delete (node);
    }
}

strid_t CompilerCore::getOrCreateID(const char *text) {
    for (strid_t id = 0; id < names_pool.size(); ++id) {
        if (std::strcmp(names_pool[id].c_str(), text) == 0) {
            return id;
        }
    }
    names_pool.emplace_back(text);
    return names_pool.size() - 1;
}

void CompilerCore::generateIR() {
    // Global scope
    IRG.genStartPrologue();
    IRG.emitAST(getRootNode());
    IRG.genStartEpilogue();
}

ASTNode *CompilerCore::getRootNode() { return ast_root; }

void CompilerCore::setRootNode(ASTNode *node) { ast_root = node; }

std::string &CompilerCore::getStringByID(strid_t id) { return names_pool[id]; }

void CompilerCore::enterScope() {
    if (!scope_stack.empty()) {
        scope_stack.back().yieldInsertBlock();
    }
    scope_stack.emplace_back();
}

void CompilerCore::leaveScope() {
    scope_stack.pop_back();
    if (!scope_stack.empty()) {
        scope_stack.back().applyInsertBlock();
    }
}

IRValue CompilerCore::lookup(strid_t id) {
    for (auto scope_it = scope_stack.rbegin(); scope_it != scope_stack.rend();
         ++scope_it) {
        if (scope_it->contains(id)) {
            return scope_it->getVariableValue(id);
        }
    }
    return IRGenerator::NIL();
}

bool CompilerCore::inGlobalScope() { return scope_stack.size() == 1; }

void CompilerCore::populateAvailableVariables(
    const std::vector<strid_t> &id_list) {
    for (strid_t id : id_list) {
        populateAvailableVariables(id);
    }
}

void CompilerCore::populateAvailableVariables(strid_t id) {
    Scope &curr_scope = scope_stack.back();
    curr_scope.setOrUpdateVariable(id);
}

void CompilerCore::addIDValueMapping(strid_t id, IRValue value) {
    Scope &curr_scope = scope_stack.back();
    assert(curr_scope.contains(id) &&
           "Mapping ID that is not available in this scope");
    curr_scope.setOrUpdateVariable(id, value);
}

IDNode *CompilerCore::makeID(const char *text) {
    strid_t id = getOrCreateID(text);
    return allocateNode<IDNode>(id);
}

numb_t CompilerCore::convertTextToNumber(const char *text) {
    char *endptr = nullptr;
    double value = std::strtod(text, &endptr);
    if (errno == ERANGE) {
        std::cerr << "ERROR : Number out of range : " << text << "\n";
        exit(EXIT_FAILURE);
    }
    if (!value && endptr == text) {
        // This means that lex rule is most likely incorrect
        std::cerr << "ERROR : Invalid number : " << text << "\n";
        assert(0 && "Number conversion failed");
        exit(EXIT_FAILURE);
    }
    // Fix precicion integer arithmetic
    return static_cast<numb_t>(value * kolang::PRECISION);
}

}; // namespace kolang
