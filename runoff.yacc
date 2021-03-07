%{
#include <stdio.h>

extern int yylineno;
extern int yylex(void);
int yyerror(const char*);
%}

/* Give me the good errors please */
%define parse.error verbose

%union {
				int int_8;
				char *id;
				}

%start START

/* string identifier */
%token <id> IDENTIFIER
/* Reserved keywords */
%token print
/* datatypes */
%token int_8 int_16 int_32 bool


%%

START: STRING_LITERAL
		 | CONST
		 | STATEMENTS
		 ;

STRING_LITERAL: IDENTIFIER;

CONST: "const" IDENTIFIER int_8;

TYPE: TYPE[int_8]
		| int_8
		| int_16
		| int_32
		| bool
		; /* HEHE ingen enum :sunglasses */


OPERATOR: '+'
				 | '-'
				 | '/'
				 | '*'
				 | "||"
				 | "&&"
				 ;


ASSIGNMENT: TYPE STRING_LITERAL '=' EXPRESSION
					;

EXPRESSION: IDENTIFIER
					| IDENTIFIER OPERATOR EXPRESSION /* a '+' b '-' c, a, a+b */

STATEMENTS: STATEMENT STATEMENTS | ;
STATEMENT: print '(' STRING_LITERAL ')' ';' /* Add support for code line here.... what to do... :Angrsad: */
				 | ASSIGNMENT
				 ;

%%

int
yyerror(const char *err)
{
	printf("Parse error at line %d: %s\n", yylineno, err);
}
