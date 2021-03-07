%{
#include <stdio.h>

extern int yylineno;
extern int yylex(void);
int yyerror(const char*);
%}

/* Give me the good errors please */
%define parse.error verbose

%union {
				char *id;
				}

%start Program
/* Reserved keywords */
%token
	CONST
 	RIGHT_ARROW
	IF
	TASK
	FOREVER
	WHILE
	FOR
	END
	COMMA
	THEN
	OPEN_PARENTHESIS
	CLOSE_PARENTHESIS
	OPEN_BRACKET
	CLOSE_BRACKET
	EQUAL
	STAR
	SLASH
	PLUS
	MINUS
/* datatypes */

%token int_literal string_literal string
%token int_8 int_16 int_32 bool


%%

Program: Decleration
			 ;

Decleration: Decleration_const END Decleration
					 | Decleration_variable END Decleration
					 | %empty
					 ;

Decleration_const: CONST string_literal int_literal
								 ;

Decleration_variable: Type string_literal
										| Type string_literal EQUAL Expr
										;


Expr: Term
		| Term '+' Expr
		| Term '-' Expr
		| Term '/' Expr
		| Term '*' Expr
		| %empty
		;

Term: string_literal
		| int_literal
		;

Operator: PLUS
				| MINUS
				| SLASH
				| STAR
				;

Type: int_8
		| int_16
		| int_32
		;




%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
