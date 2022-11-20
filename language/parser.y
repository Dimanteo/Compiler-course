%{
#include <stdio.h>

extern "C" void yyerror(const char *s);
extern "C" int yylex();
extern FILE *yyin;
%}

%token NUMBER
%token ADD SUB DIV MUL
%token END

%%

Program : Close
    | AddSubExp Close { printf("= %d\n", $1); YYACCEPT; }
;

Close : END | YYEOF

AddSubExp : MulDivExp
    | AddSubExp ADD MulDivExp { $$ = $1 + $3; }
    | AddSubExp SUB MulDivExp { $$ = $1 - $3; }
;

MulDivExp : ProrityExp
    | ProrityExp MUL ProrityExp { $$ = $1 * $3; }
    | ProrityExp DIV ProrityExp { $$ = $1 / $3; }
;

ProrityExp : NUMBER
    | '(' AddSubExp ')' { $$ = $2; }
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