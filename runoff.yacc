%{
#include <stdio.h>

extern int yylineno;
extern int yylex(void);
int yyerror(const char*);
%}

/* Give me the good errors please */
%define parse.error verbose

%union {
				char *string;
}


%token
			const_keyword
			int_literal
			float_literal
			bool_literal

			and_op
			or_op

			identifier
			function
			while_keyword
			builtin_type
			right_arrow

%left and_op or_op
%left '+' '-'
%left '*' '/'

%%

Program: Toplevels
			 ;


Toplevels: Toplevel
				 | Toplevels Toplevel
				 ;

Toplevel: DefineConst
				| DefineFunction
				/*| DefineMessageBlock
				| DefineStruct

				| DefineTask*/
				;

DefineConst: const_keyword identifier int_literal ';'
					 ;

DefineFunction: function identifier '(' ParametersList ')' right_arrow Type Codeblock
							;

ParametersList: Parameters
 				| %empty
				;

Parameters: Parameter
		| Parameters ',' Parameter
		;

Parameter: Type identifier
	 ;


ArgsList: Args
				| %empty
				;

Args: Args ',' Expression
		| Expression
		;

Type: builtin_type
		/*| struct identifier */
		;


Codeblock: '{' Statements '}'
				 ;

Statements: Statements Statement
				 | %empty
				 ;
Statement: Assignment ';'
				 | Declaration ';'
				 | Expression ';'
				 | while_keyword '(' Expression ')' Codeblock

				 ;



Assignment: identifier '=' Expression
					;

Declaration: Type identifier
					 | Type identifier '=' Expression
					 ;

Literal: int_literal
			 | float_literal
			 | bool_literal
			 ;

Expression: identifier
					| Literal
					| Expression '+' Expression
					| Expression '-' Expression
					| Expression '*' Expression
					| Expression '/' Expression
					| Expression and_op Expression
					| Expression or_op Expression
					| '!' Expression
					| identifier '(' ArgsList ')'
					;



%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
