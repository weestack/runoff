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

DefineFunction: function identifier '(' ArgsList ')' right_arrow Type Codeblock
							;
ArgsList: Args
 				| %empty
				;

Args: Arg
		| Args ',' Arg
		;

Arg: Type identifier
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

Expression: Term
					| Expression BinaryOperator Term
					;

Term: identifier
		| Literal
		;


BinaryOperator: '+'
							| '/'
							| '-'
							| '*'
							| or_op
							| and_op
							;

%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
