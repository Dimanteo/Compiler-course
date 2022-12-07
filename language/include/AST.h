#ifndef KOLANG_AST_H
#define KOLANG_AST_H

#include "IRGenerator.h"

namespace kolang {

class ASTNode {
  public:
    // Get null node
    static constexpr ASTNode *NIL() { return nullptr; }
    static bool isNIL(ASTNode *node) { return node == nullptr; }
    virtual IRValue emit() = 0;
    virtual ~ASTNode() = default;
};

template <class T> class ListNode : public ASTNode {
    ListNode *next_node;
    T *value;

  public:
    ListNode(ASTNode *node)
        : next_node(dynamic_cast<ListNode *>(ASTNode::NIL())),
          value(dynamic_cast<T *>(node)) {}
    ListNode *next() { return next_node; }
    void setNext(ListNode *other) { next_node = other; }
    bool isLast() const { return ASTNode::isNIL(next_node); }
    IRValue emit() override {
        IRValue result = value->emit();
        if (!isLast()) {
            next()->emit();
        }
        return result;
    }
    std::vector<T *> emitAsList() {
        std::vector<T *> values_vector(1, value);
        ListNode *current = this;
        while (!current->isLast()) {
            current = current->next();
            values_vector.push_back(current->value);
        }
        return values_vector;
    }
    virtual ~ListNode() = default;
};

class IDNode : public ASTNode {
    strid_t id;

  public:
    IDNode(strid_t string_id) : id(string_id) {}
    strid_t getID() { return id; }
    IRValue emit() override { return IRGenerator::NIL(); };
    virtual ~IDNode() = default;
};

using ExprNode = ListNode<ASTNode>;

using IDListNode = ListNode<IDNode>;

class BinaryOpBase : public ASTNode {
  protected:
    ASTNode *left;
    ASTNode *right;

  public:
    BinaryOpBase(ASTNode *lhs, ASTNode *rhs) : left(lhs), right(rhs) {}
    virtual ~BinaryOpBase() = default;
};

class VarNode : public ASTNode {
    IDNode *name;

  public:
    VarNode(ASTNode *node) : name(dynamic_cast<IDNode *>(node)) {}
    IRValue emit() override;
    strid_t getID() { return name->getID(); }
    virtual ~VarNode() = default;
};

class ScopeNode : public ASTNode {
    ExprNode *body;

  public:
    ScopeNode(ASTNode *node) : ScopeNode(dynamic_cast<ExprNode *>(node)) {}
    ScopeNode(ExprNode *node) : body(node) {}
    IRValue emit() override;
    virtual ~ScopeNode() = default;
};

class AssignmentNode : public ASTNode {
    VarNode *variable;
    ASTNode *value;

  public:
    AssignmentNode(ASTNode *var, ASTNode *data)
        : AssignmentNode(dynamic_cast<VarNode *>(var), data) {}
    AssignmentNode(VarNode *var, ASTNode *data) : variable(var), value(data) {}
    IRValue emit() override;
    virtual ~AssignmentNode() = default;
};

class FunctionDefNode : public ASTNode {
    IDNode *name;
    IDListNode *arguments;
    ExprNode *body;

  public:
    FunctionDefNode(ASTNode *fname, ASTNode *args, ASTNode *code)
        : FunctionDefNode(dynamic_cast<IDNode *>(fname),
                          dynamic_cast<IDListNode *>(args),
                          dynamic_cast<ExprNode *>(code)) {}
    FunctionDefNode(IDNode *fname, IDListNode *args, ExprNode *code)
        : name(fname), arguments(args), body(code) {}
    IRValue emit() override;
    virtual ~FunctionDefNode() = default;
};

class FunctionCallNode : public ASTNode {
    IDNode *function;
    ExprNode *arguments;

  public:
    FunctionCallNode(ASTNode *name, ASTNode *args)
        : FunctionCallNode(dynamic_cast<IDNode *>(name),
                           dynamic_cast<ExprNode *>(args)) {}
    FunctionCallNode(IDNode *name, ExprNode *args)
        : function(name), arguments(args) {}
    IRValue emit() override;
    virtual ~FunctionCallNode() = default;
};

class ReturnNode : public ASTNode {
    ASTNode *retval;

  public:
    ReturnNode();
    ReturnNode(ASTNode *value) : retval(value) {}
    IRValue emit() override;
    virtual ~ReturnNode() = default;
};

class AddNode : public BinaryOpBase {
  public:
    AddNode(ASTNode *lhs, ASTNode *rhs) : BinaryOpBase(lhs, rhs) {}
    virtual IRValue emit() override;
    virtual ~AddNode() = default;
};

class SubNode : public BinaryOpBase {
  public:
    SubNode(ASTNode *lhs, ASTNode *rhs) : BinaryOpBase(lhs, rhs) {}
    virtual IRValue emit() override;
    virtual ~SubNode() = default;
};

class MulNode : public BinaryOpBase {
  public:
    MulNode(ASTNode *lhs, ASTNode *rhs) : BinaryOpBase(lhs, rhs) {}
    IRValue emit() override;
    virtual ~MulNode() = default;
};

class DivNode : public BinaryOpBase {
  public:
    DivNode(ASTNode *lhs, ASTNode *rhs) : BinaryOpBase(lhs, rhs) {}
    IRValue emit() override;
    virtual ~DivNode() = default;
};

class NumberNode : public ASTNode {
    numb_t number;

  public:
    NumberNode(numb_t num) : number(num) {}
    numb_t getNumber() { return number; }
    IRValue emit() override;
    virtual ~NumberNode() = default;
};

} // namespace kolang

#endif