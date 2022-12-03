#include "CompilerCore.h"

namespace kolang {

CompilerCore::CompilerCore() {
    global_frame.reset(new GlobalFrame());
    frame_stack.push_back(global_frame.get());
}

void CompilerCore::handleAssignment(ASTNode id_node, ASTNode val) {
    strid_t id = ASTNode::asID(id_node.get());
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
    strid_t id = ASTNode::asID(id_node.get());
    for (auto frame_it = frame_stack.rbegin(); frame_it != frame_stack.rend();
         ++frame_it) {
        ASTNode memLoc = (*frame_it)->lookup(id);
        if (!memLoc.isNIL()) {
            return IRG.genLoad(memLoc);
        }
    }
    return ASTNode::getNIL();
}

strid_t CompilerCore::registerName(const char *name) {
    auto id_it = names_pool.find(name);
    if (id_it != names_pool.end()) {
        return id_it->second;
    }
    strid_t id = getNewID();
    names_pool[name] = id;
    return id;
}

std::string CompilerCore::getNameByID(strid_t id) const {
    for (auto name_it : names_pool) {
        if (name_it.second == id) {
            return name_it.first;
        }
    }
    return "";
}

Frame &CompilerCore::getCurrentFrame() const { return *frame_stack.back(); }

Frame &CompilerCore::getGLobalFrame() const { return *global_frame; }

void CompilerCore::defineFunction(ASTNode name_node, ASTNode args_node) {
    strid_t id = ASTNode::asID(name_node.get());
    const std::string &name = getNameByID(id);
    IRG.genFunction(name, args_node);
    FunctionFrame &fframe =
        functions.emplace_back(IRG.getModule().getFunction(name));
    frame_stack.push_back(&fframe);
    if (!args_node.isNIL()) {
        fframe.allocArgs(args_node);
    }
}

void CompilerCore::leaveFunction() { leaveScope(); }

void CompilerCore::enterScope() {
    scopes.emplace_back(frame_stack.back()->getCurrentBB());
    frame_stack.push_back(&scopes.back());
}

void CompilerCore::leaveScope() {
    frame_stack.pop_back();
    IRG.insertIn(frame_stack.back());
}

}; // namespace kolang
