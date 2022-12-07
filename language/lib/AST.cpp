#include "AST.h"
#include "IRGenerator.h"
#include <CompilerCore.h>

#include <cassert>
#include <string>
#include <vector>

namespace kolang {

IRValue ScopeNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.enterScope();
    IRValue emitted_body = body->emit();
    cc.leaveScope();
    return emitted_body;
}

IRValue AssignmentNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    strid_t id = variable->getID();
    IRValue var = cc.lookup(id);
    if (IRGenerator::isNIL(var)) {
        if (cc.inGlobalScope()) {
            std::string &name = cc.getStringByID(id);
            var = cc.getIRG().allocateGlobal(name);
        } else {
            var = cc.getIRG().allocateLocal();
        }
        cc.populateAvailableVariables(id);
        cc.addIDValueMapping(id, var);
    }
    assert(!IRGenerator::isNIL(var) && "Failed to get variable");
    cc.getIRG().genStore(var, value->emit());
    return var;
}

IRValue FunctionDefNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    std::string &func_name = cc.getStringByID(name->getID());
    cc.enterScope();
    if (ASTNode::isNIL(arguments)) {
        cc.getIRG().declFunction(func_name);
    } else {
        std::vector<IDNode *> arg_nodes = arguments->emitAsList();
        std::vector<strid_t> arg_ids;
        for (IDNode *node : arg_nodes) {
            arg_ids.push_back(node->getID());
        }
        cc.getIRG().declFunction(func_name, arg_ids);
    }
    std::vector<ASTNode *> body_exprs = body->emitAsList();
    for (ASTNode *expr : body_exprs) {
        expr->emit();
    }
    cc.leaveScope();
    return nullptr;
}

IRValue FunctionCallNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    std::string &func_name = cc.getStringByID(function->getID());
    if (ASTNode::isNIL(arguments)) {
        return cc.getIRG().genCall(func_name);
    }
    std::vector<ASTNode *> args = arguments->emitAsList();
    std::vector<IRValue> args_values;
    for (ASTNode *node : args) {
        args_values.push_back(node->emit());
    }
    IRValue retval = cc.getIRG().genCall(func_name, args_values);
    if (IRGenerator::isNIL(retval)) {
        return IRGenerator::NIL();
    }
    return retval;
}

ReturnNode::ReturnNode() {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.make<NumberNode>(0);
}

IRValue ReturnNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRG.genReturn(retval->emit());
    return IRGenerator::NIL();
}

IRValue AddNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genAdd(left->emit(), right->emit());
}

IRValue SubNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genSub(left->emit(), right->emit());
}

IRValue MulNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genMul(left->emit(), right->emit());
}

IRValue DivNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genDiv(left->emit(), right->emit());
}

IRValue VarNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    IRValue var = cc.lookup(name->getID());
    if (IRGenerator::isNIL(var)) {
        return var;
    }
    return cc.getIRG().genLoad(var);
}

IRValue NumberNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genNumber(number);
}

IRValue IfNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRG.genIf(condition->emit(), true_path);
    return IRGenerator::NIL();
}

IRValue LTNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genLT(left->emit(), right->emit());
}
IRValue GTNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genGT(left->emit(), right->emit());
}

IRValue EQNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genEQ(left->emit(), right->emit());
}

IRValue NEQNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genNEQ(left->emit(), right->emit());
}

IRValue LEQNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genLEQ(left->emit(), right->emit());
}

IRValue GEQNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genGEQ(left->emit(), right->emit());
}

} // namespace kolang