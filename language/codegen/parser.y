%{
#include "CompilerCore.h"
#include "AST.h"
#include <iostream>

#define YYSTYPE kolang::ASTNode *

using namespace kolang;

extern "C" void yyerror(const char *s);
extern "C" int yylex();
extern FILE *yyin;
extern int yylineno;

%}

%token NUMBER
%token ADD SUB DIV MUL
%token ENDLN
%token ID
%token LT GT EQ NEQ LEQ GEQ
%token ASSIGN
%token BRA KET FIGBRA FIGKET
%token COMA
%token RET

%%

Language : Program { 
        CompilerCore &cc = CompilerCore::getCCore();
        cc.setRootNode($1);
        YYACCEPT; 
    }
;

Program : GlobalDef {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<ExprNode>($1);
    } 
    | GlobalDef Program {
        CompilerCore &cc = CompilerCore::getCCore();
        ExprNode *expr = cc.make<ExprNode>($1);
        expr->setNext(dynamic_cast<ExprNode *>($2));
        $$ = expr;
    }
;

GlobalDef : FunctionDef | VarAssignment ENDLN;

FunctionDef : ID ArgsDef FIGBRA FunctionBody FIGKET {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ =  cc.make<FunctionDefNode>($1, $2, $4);
    }
;

ArgsDef : BRA NameList KET {
        $$ = $2;
    }
    | BRA KET {
        $$ = ASTNode::NIL();
    }
;

NameList : NameList COMA ID {
        CompilerCore &cc = CompilerCore::getCCore();
        IDListNode *id = cc.make<IDListNode>($3);
        IDListNode *list = dynamic_cast<IDListNode *>($1);
        list->setNext(id);
        $$ = list;
    }
    | ID {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<IDListNode>($1);
    }
;

FunctionBody : Expression ENDLN FunctionBody {
        CompilerCore &cc = CompilerCore::getCCore();
        ExprNode *exprList = cc.make<ExprNode>($1);
        exprList->setNext(dynamic_cast<ExprNode *>($3));
        $$ = exprList;
    }
    | Expression ENDLN {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<ExprNode>($1);
    }
;

Expression : VarAssignment | FunctionCall | Statement;

Statement : RET { 
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<ReturnNode>();
    }
    | RET ArithmeticExpr {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<ReturnNode>($2);
    }
;

// LogicExpression : ArithmeticExpr EQ ArithmeticExpr {
//         IRGenerator &irg = CompilerCore::getCCore().getIRG();
//         $$ = irg.genCmpEq($1, $3);
//     }
//     | ArithmeticExpr NEQ ArithmeticExpr {
//         $$ = irg.genCmpNeq($1, $3);
//     }
//     | ArithmeticExpr LEQ ArithmeticExpr {
//         $$ = irg.genCmpLeq($1, $3);
//     }
//     | ArithmeticExpr GEQ ArithmeticExpr {
//         $$ = irg.genCmpGeq($1, $3);
//     }
//     | ArithmeticExpr LT ArithmeticExpr {
//         $$ = irg.genCmpLt($1, $3);
//     }
//     | ArithmeticExpr GT ArithmeticExpr {
//         $$ = irg.genCmpGt($1, $3);
//     }
// ;

VarAssignment : ID ASSIGN ArithmeticExpr {
    CompilerCore &cc = CompilerCore::getCCore();
    VarNode *var = cc.make<VarNode>($1);
    $$ = cc.make<AssignmentNode>(var, $3);
}
;

ArithmeticExpr : FactorExpr
    | ArithmeticExpr ADD FactorExpr { 
        // $$ $1 + $3;
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<AddNode>($1, $3); 
    }
    | ArithmeticExpr SUB FactorExpr { 
        // $$ = $1 - $3;
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<SubNode>($1, $3);
    }
;

FactorExpr : BracketsExpr
    | FactorExpr MUL BracketsExpr { 
        // $$ = $1 * $3; 
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<MulNode>($1, $3);
    }
    | FactorExpr DIV BracketsExpr { 
        // $$ = $1 / $3;
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<DivNode>($1, $3);
    }
;

BracketsExpr : BRA ArithmeticExpr KET { $$ = $2; }
    | NUMBER | FunctionCall | VarUse
;

FunctionCall : ID ArgsPassing {
    CompilerCore &cc = CompilerCore::getCCore();
    $$ = cc.make<FunctionCallNode>($1, $2);
}
;

ArgsPassing : BRA ValueList KET {
        $$ = $2;
    }
    | BRA KET { $$ = ASTNode::NIL(); }
;

VarUse : ID {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<VarNode>($1);
    }
;

ValueList : ArithmeticExpr {
        CompilerCore &cc = CompilerCore::getCCore();
        $$ = cc.make<ExprNode>($1);
    }
    | ValueList COMA ArithmeticExpr {
        CompilerCore &cc = CompilerCore::getCCore();
        ExprNode *list = cc.make<ExprNode>($3);
        list->setNext(dynamic_cast<ExprNode *>($1));
        $$ = list;
    }
;

%%

bool parser_main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }
    return yyparse();
}

void yyerror(const char *s) {
    std::cerr << "Parser error: " << s << " in line " << yylineno << "\n";
}
