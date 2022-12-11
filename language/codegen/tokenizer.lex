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

[#].*\n ; // comment
[;]     { return ENDLN; }
[+]     { return ADD; }
[-]     { return SUB; }
[*]     { return MUL; }
[/]     { return DIV; }
[(]     { return BRA; }
[)]     { return KET; }
[{]     { return FIGBRA; }
[}]     { return FIGKET; }
[[]     { return SQBRA; }
[]]     { return SQKET; }
[=][=]    { return EQ; }
[<][=]    { return LEQ; }
[>][=]    { return GEQ; }
[!][=]    { return NEQ; }
[<]     { return LT; }
[>]     { return GT; }
[=]     { return ASSIGN; }
[0-9]+(\.[0-9]+)*   { 
    CompilerCore &cc = CompilerCore::getCCore();
    numb_t val = cc.convertTextToNumber(yytext);
    yylval = cc.make<NumberNode>(val);
    return NUMBER; 
}
[l][e][t] { return VARDEF; }
[r][e][t][u][r][n] { return RET; }
[i][f] {return IF; }
[w][h][i][l][e] { return WHILE; }
[A-Za-z_]+[0-9A-Za-z_]* {
    CompilerCore &cc = CompilerCore::getCCore();
    yylval  = cc.makeID(yytext);
    return ID;
}
[,] { return COMA; }
[ \n\t\r] ; // whitespace
. { return *yytext; }
%%