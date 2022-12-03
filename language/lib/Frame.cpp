#include "Frame.h"
#include "CompilerCore.h"

namespace kolang {

ASTNode Frame::lookup(id_t id) const {
    auto node_it = locals_map.find(id);
    if (node_it == locals_map.end()) {
        return ASTNode::getNIL();
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
    if (memLoc.isNIL()) {
        return false;
    }
    IRG.genStore(memLoc, value);
    return true;
}

/****** Global Frame ******/
llvm::BasicBlock *GlobalFrame::getCurrentBB() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    llvm::Function *func =
        IRG.getModule().getFunction(IRGenerator::getStartFuncName());
    return &func->getEntryBlock();
}

ASTNode GlobalFrame::allocVariable(id_t id) {
    CompilerCore &cc = CompilerCore::getCCore();
    IRGenerator &IRG = cc.getIRG();
    std::string name = cc.getNameByID(id);
    ASTNode memLoc = IRG.genGlobalAlloc(name);
    addVariable(id, memLoc);
    return memLoc;
}

/****** Function Frame ******/

FunctionFrame::FunctionFrame(llvm::Function *func) : function(func) {
    assert(func && "Invalid function passed as argument");
    blocks.push_back(&func->getEntryBlock());
}

void FunctionFrame::allocArgs(ASTNode &argv) {
    for (size_t argi = 0; argi < argv.getNumValues(); ++argi) {
        strid_t id = ASTNode::asID(argv[argi]);
        ASTNode memLoc = allocVariable(id);
        CompilerCore::getCCore().getIRG().genStore(memLoc,
                                                   function->getArg(argi));
    }
}

}; // namespace kolang