%{
#include "IRGenerator.h"

#include <stdlib.h>

#include "parser.hpp"

using namespace codak;

extern "C" int yylex();
%}

%option yylineno

%%

[+]     { return ADD; }
[-]     { return SUB; }
[*]     { return MUL; }
[/]     { return DIV; }
[0-9]+   { 
    int val = atoi(yytext);
    IRGenerator &irg = IRGenerator::getGenerator();
    yylval = irg.genNumber(val);
    return NUMBER; 
}
[\n]     { return END; }
[ \t\r] ; // whitespace
. { return *yytext; }
%%