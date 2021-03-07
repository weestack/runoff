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
%token int_literal

%type <int_8> int_literal


%%

START: STRING_LITERAL
		 | CONST
		 | STATEMENTS
		 ;

STRING_LITERAL: IDENTIFIER;

CONST: "const" IDENTIFIER int_8;

TYPE: TYPE'[' int_literal ']'
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

COMPOUND_STATEMENT: '{' STATEMENTS '}'
									|  '{' COMPOUND_STATEMENT '}'
									;


FUNCTION: STRING_LITERAL '(' terms ')' ';'

FUNCTION_DEFINE: 'function' STRING_LITERAL '(' terms ')' COMPOUND_STATEMENT




/*
START: DECLERATIVE
		 ;


DECLERATIVE: DECLERATIVE_CONSTS
					 ;

DECLERATIVE_CONSTS: DECLERATIVE_CONST DECLERATIVE_CONSTS
                  | %empty
									;

DECLERATIVE_CONST: CONST string_literal int_literal END
								 ;

TERMS: TERM TERMS

TERM: int_literal
		| int_literal '+' TERM
		| int_literal '-' TERM
		| int_literal '/' TERM
		| int_literal '*' TERM
		;

TYPE: int_8
		| int_16
		| int_32
		| bool
		; /* HEHE ingen enum :sunglasses */

*/

%%

int
yyerror(const char *err)
{
	printf("Parse error at line %d: %s\n", yylineno, err);
}
