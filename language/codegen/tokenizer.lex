%{
#include "CompilerCore.h"

#include <iostream>

#define YYSTYPE kolang::ASTNode *

#include "parser.hpp"

using namespace kolang;

extern "C" int yylex();
%}

%option yylineno

%%

[;]     { return ENDLN; }
[+]     { return ADD; }
[-]     { return SUB; }
[*]     { return MUL; }
[/]     { return DIV; }
[(]     { return BRA; }
[)]     { return KET; }
[{]     { return FIGBRA; }
[}]     { return FIGKET; }
[=][=]    { return EQ; }
[<][=]    { return LEQ; }
[>][=]    { return GEQ; }
[!][=]    { return NEQ; }
[<]     { return LT; }
[>]     { return GT; }
[=]     { return ASSIGN; }
[0-9]+   { 
    int64_t val = atoi(yytext);
    CompilerCore &cc = CompilerCore::getCCore();
    yylval = cc.make<NumberNode>(val);
    return NUMBER; 
}
[r][e][t][u][r][n] { return RET; }
[i][f] {return IF; }
[A-Za-z_]+[0-9A-Za-z_]* {
    CompilerCore &cc = CompilerCore::getCCore();
    yylval  = cc.makeID(yytext);
    return ID;
}
[,] { return COMA; }
[ \n\t\r] ; // whitespace
. { return *yytext; }
%%