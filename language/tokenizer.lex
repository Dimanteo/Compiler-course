%{
#include <parser.hpp>
#include <stdlib.h>
extern "C" int yylex();
%}

%%

[+]     { return ADD; }
[-]     { return SUB; }
[*]     { return MUL; }
[/]     { return DIV; }
[0-9]+   { yylval = atoi(yytext); return NUMBER; }
[\n]     { return END; }
[ \t\r] ; // whitespace
. { return *yytext; }
%%