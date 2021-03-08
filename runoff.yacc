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
			struct_keyword
			int_literal
			float_literal
			bool_literal

			and_op
			or_op
			greater_equal
			less_equal
			equal

			increment
			decrement

			identifier
			function
			task
			while_keyword
			for_keyword
			builtin_type
			right_arrow

%left and_op or_op
%left equal less_equal greater_equal '<' '>'
%left '+' '-'
%left '*' '/'
%left increment decrement

%right '!' '='
%%

Program: Toplevels
			 ;


Toplevels: Toplevel
				 | Toplevels Toplevel
				 ;

Toplevel: DefineConst
				| DefineFunction
				| DefineTask
				| DefineStruct
				/*| DefineMessageBlock */
				;

DefineConst: const_keyword identifier int_literal ';'
					 ;

DefineFunction: function identifier '(' ParametersList ')' right_arrow Type Codeblock
							;

DefineTask: task identifier '(' ParametersList ')' Codeblock
					;


DefineStruct: struct_keyword identifier '{' StructMembers '}'
						;
						
StructMembers: StructMembers StructMember
						 ;

StructMember: Type identifier ';'
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
Statement: Expression ';'
				 | while_keyword '(' Expression ')' Codeblock
				 | for_keyword '(' MaybeExpression ';' MaybeExpression ';' MaybeExpression ')' Codeblock
				 ;

MaybeExpression: Expression
							 | %empty
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
					| Expression equal Expression
					| Expression greater_equal Expression
					| Expression less_equal Expression
					| '!' Expression
					| identifier '(' ArgsList ')'
					| identifier '=' Expression
					| identifier increment
					| identifier decrement
					| Declaration
					;



%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
