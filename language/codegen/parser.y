%{
#include "CompilerCore.h"
#include <iostream>

#define YYSTYPE kolang::ASTNode

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
%token LE GT EQ NEQ LEQ GEQ
%token ASSIGN
%token BRA KET FIGBRA FIGKET
%token COMA
%token RET

%%

Language : Program { 
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        if (irg.finalizeStartFunction()) {
            YYACCEPT; 
        } else {
            YYERROR;
        }
    }

Program : GlobalDef | Program GlobalDef;

GlobalDef : FunctionDef | VarAssignment ENDLN;

FunctionDef : ID ArgsDef FIGBRA  {
        CompilerCore &cc = CompilerCore::getCCore();
        cc.defineFunction($1, $2);
    } Body FIGKET {
        CompilerCore &cc = CompilerCore::getCCore();
        cc.leaveFunction();
    }
;

ArgsDef : BRA NameList KET {
        $$ = $2;
    }
    | BRA KET {
        $$ = ASTNode::getNIL();
    }

NameList : NameList COMA ID {
        ASTNode &argv = $1;
        argv.addValue($3);
        $$ = argv;
    }
    | ID
;

Scope : FIGBRA {
        CompilerCore &cc = CompilerCore::getCCore();
        cc.enterScope();
    } Body FIGKET {
        CompilerCore &cc = CompilerCore::getCCore();
        cc.leaveScope();
    }
;

Body : ENDLN | Expression ENDLN | Body Expression ENDLN;

Expression : VarAssignment | FunctionCall | Statement | Scope;

Statement : RET { 
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        irg.genRet();
    }
    | RET ArithmeticExpr {
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        irg.genRet($2);
    }
;

VarAssignment : ID ASSIGN ArithmeticExpr {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.handleAssignment($1, $3);
}
;

ArithmeticExpr : FactorExpr
    | ArithmeticExpr ADD FactorExpr { 
        // $$ $1 + $3;
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genAdd($1, $3); 
    }
    | ArithmeticExpr SUB FactorExpr { 
        // $$ = $1 - $3;
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genSub($1, $3);
    }
;

FactorExpr : BracketsExpr
    | FactorExpr MUL BracketsExpr { 
        // $$ = $1 * $3; 
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genMul($1, $3);
    }
    | FactorExpr DIV BracketsExpr { 
        // $$ = $1 / $3;
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genDiv($1, $3);
    }
;

BracketsExpr : BRA ArithmeticExpr KET { $$ = $2; }
    | Literal
;

Literal : ID {
        CompilerCore &cc = CompilerCore::getCCore();
        ASTNode var = cc.handleVarUse($1);
        if (var.isNIL()) {
            std::cerr << "Syntax ERROR: Undefined variable at line "
                      << yylineno << ".\n";
            YYERROR;
        }
        $$ = var;
    }
    | FunctionCall
    | NUMBER
;

FunctionCall : ID ArgsPassing {
    CompilerCore &cc = CompilerCore::getCCore();
    ASTNode name_node = $1;
    std::string name = cc.getNameByID(ASTNode::asID(name_node.get()));
    $$ = cc.getIRG().genCall(name, $2);
}
;

ArgsPassing : BRA ExprList KET {
        $$ = $2;
    }
    | BRA KET { $$ = ASTNode::getNIL(); }
;

ExprList : ArithmeticExpr
    | ExprList COMA ArithmeticExpr {
        ASTNode &argv = $1;
        argv.addValue($3);
        $$ = argv;
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
