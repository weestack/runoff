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
			identifier


%%

Program: Toplevels
			 ;


Toplevels: Toplevel
				 | Toplevels Toplevel
				 ;

Toplevel: DefineConst
				/*| DefineMessageBlock
				| DefineStruct
				| DefineFunction
				| DefineTask*/
				;

DefineConst: const_keyword identifier int_literal ';'

%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
