#include "AST.h"
#include "IRGenerator.h"
#include <CompilerCore.h>

#include <cassert>
#include <string>
#include <vector>

namespace kolang {

bool isVariableType(TYPE_ID type) { return type == TYPE_ID::INT64; }

bool isArrayType(TYPE_ID type) { return type == ARR_INT64; }

IRValue ScopeNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.enterScope();
    IRValue emitted_body = body->emit();
    cc.leaveScope();
    return emitted_body;
}

IRValue AssignmentNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRValue result = variable->emit();
    IRG.genStore(result, value->emit());
    return result;
}

IRValue FunctionDefNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    std::string &func_name = cc.getStringByID(name->getID());
    cc.enterScope();
    if (ASTNode::isNIL(arguments)) {
        cc.getIRG().declFunction(func_name);
    } else {
        std::vector<TypeIDNode *> arg_nodes = arguments->emitAsList();
        cc.getIRG().declFunction(func_name, arg_nodes);
    }
    std::vector<ASTNode *> body_exprs = body->emitAsList();
    for (ASTNode *expr : body_exprs) {
        expr->emit();
    }
    cc.getIRG().genDefaultReturn();
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

IRValue VarDefNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    strid_t id = name->getID();
    IRValue var = IRGenerator::NIL();
    if (cc.inGlobalScope()) {
        std::string &name = cc.getStringByID(id);
        var = cc.getIRG().allocateGlobal(name);
    } else {
        var = cc.getIRG().allocateLocal();
    }
    cc.populateAvailableVariables(id);
    cc.addIDValueMapping(id, var);
    return var;
}

IRValue IDNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    return cc.lookup(id);
}

IRValue VarUseNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    IRValue memory = name->emit();
    if (IRGenerator::isNIL(memory)) {
        std::string &name = cc.getStringByID(getID());
        std::cerr << "ERROR : undefined variable " << name << "\n";
        exit(EXIT_FAILURE);
    }
    if (memory->getType()->getContainedType(0)->isArrayTy()) {
        return memory;
    }
    return cc.getIRG().genLoad(memory);
}

IRValue VarAccessNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    return cc.lookup(name->getID());
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

IRValue WhileNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRG.genWhile(condition, body);
    return IRGenerator::NIL();
}

IRValue ArrayDefNode::emit() {
    CompilerCore &cc = CompilerCore::getCCore();
    strid_t id = name->getID();
    IRValue array_val =
        cc.getIRG().genArrayDef(size->getNumber() / kolang::PRECISION);
    cc.populateAvailableVariables(id);
    cc.addIDValueMapping(id, array_val);
    return array_val;
}

IRValue ArrayUseNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRValue memory =
        IRG.genArrayAccess(name->getID(), IRG.normalize(index->emit()));
    return IRG.genLoad(memory);
}

IRValue ArrayAccessNode::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    return IRG.genArrayAccess(name->getID(), IRG.normalize(index->emit()));
}

IRValue LogicalBinaryOp::emit() {
    IRGenerator &IRG = CompilerCore::getCCore().getIRG();
    IRValue value = nullptr;
    switch (operation) {
    case LOGICAL_OP::OR:
        value = IRG.genLogicalOr(left->emit(), right->emit());
        break;
    case LOGICAL_OP::AND:
        value = IRG.genLogicalAnd(left->emit(), right->emit());
        break;
    default:
        llvm_unreachable("Impossible logical operation");
        break;
    }
    return value;
}

} // namespace kolang