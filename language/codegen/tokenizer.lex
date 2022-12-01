%{
#include "CompilerCore.h"

#include <iostream>

#define YYSTYPE kolang::ASTNode

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
[<]     { return LE; }
[>]     { return GT; }
[=]     { return ASSIGN; }
[0-9]+   { 
    int val = atoi(yytext);
    IRGenerator &irg = CompilerCore::getCCore().getIRG();
    yylval = irg.genNumber(val);
    return NUMBER; 
}
[p][r][i][n][t] { return PRINT; }
[r][e][t][u][r][n] { return RET; }
[A-Za-z_]+[0-9A-Za-z_]* {
    CompilerCore &cc = CompilerCore::getCCore();
    kolang::id_t id = cc.registerName(yytext);
    yylval = id;
    return ID;
} 
[ \n\t\r] ; // whitespace
. { return *yytext; }
%%