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
            messages
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
            if_keyword
            else_keyword
            elseif_keyword
            switch_keyword
            receive
            case_keyword
            default_keyword
			builtin_type
			right_arrow
            return_keyword

            include_keyword

%left and_op or_op
%left equal less_equal greater_equal '<' '>'
%left '+' '-'
%left '*' '/'
%left increment decrement

%right '!' '=' '?' ':'
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
	    | DefineMessage
        | IncludeRunoffFile
		;

DefineConst: const_keyword identifier int_literal ';'
		   ;

DefineFunction: function identifier '(' ParametersList ')' right_arrow Type Codeblock
			  ;

DefineTask: task identifier '(' ParametersList ')' Codeblock
		  ;


DefineStruct: struct_keyword identifier '{' StructMembers '}'
			;

DefineMessage: messages '{' MessageIdentifiers '}'
             ;

IncludeRunoffFile: include_keyword '(' identifier ')' ';'
             ;

MessageIdentifiers: MessageIdentifiers MessageIdentifier
                  | MessageIdentifier
                  ;

MessageIdentifier: identifier '(' Parameters ')' ';'
                 | identifier ';'
                 ;

StructMembers: StructMembers StructMember
			 | StructMember
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
    | struct_keyword identifier
    | Type '[' int_literal ']'
    | Type '[' ']'
    ;

Codeblock: '{' Statements '}'
				 ;

Statements: Statements Statement
				 | %empty
				 ;
Statement: Expression ';'
				 | while_keyword '(' Expression ')' Codeblock
				 | for_keyword '(' MaybeExpression ';' MaybeExpression ';' MaybeExpression ')' Codeblock
                 | switch_keyword '(' Expression ')' '{' SwitchCases '}'
                 | IfStatement
                 | receive '{' ReceiveCases '}'
                 | return_keyword Expression ';'
                 | return_keyword ';'
				 ;


IfStatement: if_keyword '(' Expression ')' Codeblock ElsePart

ElsePart: else_keyword Codeblock
        | elseif_keyword '(' Expression ')' Codeblock ElsePart
        | %empty

SwitchCases: SwitchCases SwitchCase
           | %empty
           ;

SwitchCase: case_keyword Literal ':' Statements
          | default_keyword ':' Statements
          ;


ReceiveCases: ReceiveCases ReceiveCase
            | %empty
            ;

ReceiveCase: case_keyword MessageIdentifier ':' Statements

MessageIdentifier: identifier
                 | identifier '(' Identifiers ')'
                 ;

Identifiers: Identifiers ',' identifier
           | identifier
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

Expression: Location
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
          | '(' Expression ')'
          | identifier '(' ArgsList ')'
          | Location '=' Expression
          | identifier increment
          | identifier decrement
          | Declaration
          | Expression '?' Expression ':' Expression
          ;


Location: identifier
        | identifier '.' Location
        | identifier Indexes
        ;

Indexes: Indexes Index
       | Index
       ;

Index: '[' int_literal ']'
     ;



%%

int
yyerror(const char *err) {
		printf("Parse error at line %d: %s\n", yylineno, err);
}
