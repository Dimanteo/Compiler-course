#include "Frame.h"
#include "CompilerCore.h"

namespace kolang {

ASTNode Frame::lookup(id_t id) const {
    auto node_it = locals_map.find(id);
    if (node_it == locals_map.end()) {
        return IRGenerator::NIL_NODE;
    }
    return (*node_it).second;
}

void Frame::addVariable(id_t id, ASTNode value) { locals_map[id] = value; }

ASTNode Frame::allocVariable(id_t id) {
    CompilerCore &cc = CompilerCore::getCCore();
    IRGenerator &IRG = cc.getIRG();
    ASTNode memLoc = IRG.genLocalAlloc();
    addVariable(id, memLoc);
    return memLoc;
}

bool Frame::assignVariable(id_t id, ASTNode value) {
    CompilerCore &cc = CompilerCore::getCCore();
    IRGenerator &IRG = cc.getIRG();
    ASTNode memLoc = lookup(id);
    if (memLoc == IRGenerator::NIL_NODE) {
        return false;
    }
    IRG.genStore(memLoc, value);
    return true;
}

ASTNode GlobalFrame::allocVariable(id_t id) {
    CompilerCore &cc = CompilerCore::getCCore();
    IRGenerator &IRG = cc.getIRG();
    std::string name = cc.getNameByID(id);
    ASTNode memLoc = IRG.genGlobalAlloc(name);
    addVariable(id, memLoc);
    return memLoc;
}

}; // namespace kolang