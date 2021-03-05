%{
#include <stdio.h>

extern int yylineno;
extern int yylex(void);
int yyerror(const char*);
%}

/* Give me the good errors please */
%define parse.error verbose

%union {char *str;}

%token a b c

%%
Prog: a b c
	;
%%

int
yyerror(const char *err)
{
	printf("Parse error at line %d: %s\n", yylineno, err);
}
