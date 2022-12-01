#include "CompilerCore.h"

namespace kolang {

CompilerCore::CompilerCore() { 
    global_frame.reset(new GlobalFrame());
    frame_stack.push_back(global_frame.get()); 
}

void CompilerCore::handleAssignment(ASTNode id_node, ASTNode val) {
    id_t id = std::get<id_t>(id_node);
    for (auto frame_it = frame_stack.rbegin(); frame_it != frame_stack.rend();
         ++frame_it) {
        if ((*frame_it)->assignVariable(id, val)) {
            return;
        }
    }
    // If we are here, than variable wasn't found in any scope
    // Create variable definition
    getCurrentFrame().allocVariable(id);
    getCurrentFrame().assignVariable(id, val);
}

ASTNode CompilerCore::handleVarUse(ASTNode id_node) {
    id_t id = std::get<id_t>(id_node);
    for (auto frame_it = frame_stack.rbegin(); frame_it != frame_stack.rend();
         ++frame_it) {
        ASTNode memLoc = (*frame_it)->lookup(id);
        if (memLoc != IRGenerator::NIL_NODE) {
            return IRG.genLoad(memLoc);
        }
    }
    return IRGenerator::NIL_NODE;
}

id_t CompilerCore::registerName(const char *name) {
    auto id_it = names_pool.find(name);
    if (id_it != names_pool.end()) {
        return id_it->second;
    }
    id_t id = getNewID();
    names_pool[name] = id;
    return id;
}

std::string CompilerCore::getNameByID(id_t id) const {
    for (auto name_it : names_pool) {
        if (name_it.second == id) {
            return name_it.first;
        }
    }
    return "";
}

Frame &CompilerCore::getCurrentFrame() const { return *frame_stack.back(); }

Frame &CompilerCore::getGLobalFrame() const { return *global_frame; }

void CompilerCore::defineFunction(ASTNode name_node) {
    id_t id = std::get<id_t>(name_node);
    const std::string &name = getNameByID(id);
    IRG.genFunction(name);
    functions.emplace_back(IRG.getModule().getFunction(name));
    frame_stack.push_back(&functions.back());
}

void CompilerCore::leaveFunction() { 
    leaveScope();
}

void CompilerCore::enterScope() {
    scopes.emplace_back(frame_stack.back()->getCurrentBB());
    frame_stack.push_back(&scopes.back());
}

void CompilerCore::leaveScope() { 
    frame_stack.pop_back();
    IRG.insertIn(frame_stack.back());
}

}; // namespace kolang
