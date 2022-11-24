%{
#include "IRGenerator.h"
#include <iostream>

using namespace codak;

extern "C" void yyerror(const char *s);
extern "C" int yylex();
extern FILE *yyin;
%}

%token NUMBER
%token ADD SUB DIV MUL
%token END

%%

Program : Close { YYACCEPT; }
    | AddSubExp Close {
        IRGenerator &irg = IRGenerator::getGenerator();
        $$ = irg.genRet($1);
        YYACCEPT;
    }
;

Close : END | YYEOF

AddSubExp : MulDivExp
    | AddSubExp ADD MulDivExp { 
        // $$ $1 + $3;
        IRGenerator &irg = IRGenerator::getGenerator();
        $$ = irg.genAdd($1, $3); 
    }
    | AddSubExp SUB MulDivExp { 
        // $$ = $1 - $3;
        IRGenerator &irg = IRGenerator::getGenerator();
        $$ = irg.genSub($1, $3);
    }
;

MulDivExp : ProrityExp
    | ProrityExp MUL ProrityExp { 
        // $$ = $1 * $3; 
        IRGenerator &irg = IRGenerator::getGenerator();
        $$ = irg.genMul($1, $3);
    }
    | ProrityExp DIV ProrityExp { 
        // $$ = $1 / $3;
        IRGenerator &irg = IRGenerator::getGenerator();
        $$ = irg.genDiv($1, $3);
    }
;

ProrityExp : '(' AddSubExp ')' { $$ = $2; }
    | NUMBER
;

%%

void parser_main(int argc, char **argv)
{
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }
    yyparse();
}

void yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}
