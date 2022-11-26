%{
// THIS CODE IS AUTO-GENERATED
// If you want to modify it refer to codegen/ files

#include "CompilerCore.h"
#include <iostream>

#define YYSTYPE kolang::ASTNode
using namespace kolang;

extern "C" void yyerror(const char *s);
extern "C" int yylex();
extern FILE *yyin;
%}

%token NUMBER
%token ADD SUB DIV MUL
%token ENDLN
%token ID
%token LE GT EQ NEQ LEQ GEQ
%token ASSIGN
%token BRA KET
%token PRINT
%token RET

%%

Language : Program { YYACCEPT; }

Program : ENDLN | Expression ENDLN | Program Expression ENDLN

Expression : VarAssignment | FunctionCall 
    | RET { 
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        irg.genRet();
    }
;

VarAssignment : ID ASSIGN ArithmeticExpr {
    CompilerCore &cc = CompilerCore::getCCore();
    cc.handleAssignment($1, $3);
}
;

ArithmeticExpr : FactorExpr
    | FactorExpr ADD FactorExpr { 
        // $$ $1 + $3;
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genAdd($1, $3); 
    }
    | FactorExpr SUB FactorExpr { 
        // $$ = $1 - $3;
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genSub($1, $3);
    }
;

FactorExpr : BracketsExpr
    | BracketsExpr MUL BracketsExpr { 
        // $$ = $1 * $3; 
        IRGenerator &irg = CompilerCore::getCCore().getIRG();
        $$ = irg.genMul($1, $3);
    }
    | BracketsExpr DIV BracketsExpr { 
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
        if (var == IRGenerator::NIL_NODE) {
            std::cerr << "Syntax ERROR: Undefined variable.\n";
            YYERROR;
        }
        $$ = var;
    }
    | NUMBER
;

FunctionCall : PRINT BRA ID KET {
        std::cout << "ID\n";
    }
    | PRINT BRA NUMBER KET {
        std::cout << "Number\n";
    }
;

%%

void parser_main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }
    yyparse();
}

void yyerror(const char *s) {
    std::cerr << "Parser error:" << s << "\n";
}
