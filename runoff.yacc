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
				char *str;
				}

%start Start

%token <id> IDENTIFIER
%token print
%token int_8 int_16 int_32 float signed unsigned struct
%token bool char void

%%

Start: STRING_LITERAL
     | CONST
		 | '(' EXPRESSION ')'
		 | STATEMENTS
		 ;

STRING_LITERAL: string /* perhaps add function name here? */
							;
CONST: string int_8 ';'
TYPE: TYPE[int_8]
		| int_8
		| int_16
		| int_32
		| float
		| CONST
		| struct
		| signed
		| unsigned
		| void
		| char
		| bool
		; /* HEHE ingen enum :sunglasses */

OPERATOR: '+'
				 | '-'
				 | '/'
				 | '*'
				 | "||"
				 | "&&"
				 ;

ASSIGNMENT: string_literal '=' EXPRESSION
					;

TYPECASTING: '(' TYPE ')' EXPRESSION ';'
					 ;

EXPRESSION: IDENTIFIER
					| IDENTIFIER OPERATOR EXPRESSION /* a '+' b '-' c, a, a+b */

STATEMENTS: STATEMENT STATEMENTS | ;
STATEMENT: print '(' STRING_LITERAL ')' ';' /* Add support for code line here.... what to do... :Angrsad: */
				 | /* Lambda */

				 ;

FUNCTION: STRING_LITERAL '(' EXPRESSION ')' FUNCTION_TYPE COMPOUNT_STATEMENT
				;

FUNCTION_TYPE: '->' TYPE
				;

COMPOUNT_STATEMENT:'{' STATEMENT '}'
 									| '{' COMPOUNT_STATEMENT '}'



%%

int
yyerror(const char *err)
{
	printf("Parse error at line %d: %s\n", yylineno, err);
}
