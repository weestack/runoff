%{
    #include <stdio.h>
    #include "ast.h"

    extern int yylineno;
    extern int yylex(void);
    int yyerror(const char*);
%}

/* Give me the good errors please */
%define parse.error verbose

%union {
    struct AstNode* astNode;
}

/* Define all non-terminals as type astNode */
%type <astNode> Program Toplevel MessageIdentifiers MessageIdentifier StructMembers StructMember ParametersList Parameters ArgsList Args Type Statements Statement ElsePart SwitchCases SwitchCase ReceiveCases ReceiveCase CallMessageIdentifier Identifiers MaybeExpression Declaration Literal Expression Location Indexes

%token
    <astNode> /* Define type */
    /* Terminals below */
    const_keyword
    struct_keyword
    messages
    int_literal
    float_literal
    bool_literal

    and_op
    or_op
    greater_than
    greater_equal
    less_than
    less_equal
    equal
    not_equal
    modulo

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
    spawn_keyword
    send_keyword
    to_keyword

    include_keyword

    bitwise_op
    bitwise_or
    bitwise_xor
    bitwise_not
    right_shift
    left_shift



%left and_op or_op bitwise_op bitwise_or bitwise_xor bitwise_not right_shift left_shift
%left equal not_equal less_equal greater_equal less_than greater_than modulo
%left '+' '-'
%left '*' '/'
%left increment decrement

%right '!' '=' '?' ':'
%%

Program: Toplevel 
       | Program Toplevel
       ;

Toplevel: const_keyword identifier int_literal ';'
        | function identifier '(' ParametersList ')' right_arrow Type '{' Statements '}'
        | task identifier '(' ParametersList ')' '{' Statements '}'
        | struct_keyword identifier '{' StructMembers '}'
        | messages '{' MessageIdentifiers '}'
        | include_keyword '(' identifier ')' ';'
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

Parameters: Type identifier
          | Parameters ','
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

Statements: Statements Statement
          | %empty
          ;

Statement: Expression ';'
         | while_keyword '(' Expression ')' '{' Statements '}'
         | for_keyword '(' MaybeExpression ';' MaybeExpression ';' MaybeExpression ')' '{' Statements '}'
         | switch_keyword '(' Expression ')' '{' SwitchCases '}'
         | if_keyword '(' Expression ')' '{' Statements '}' ElsePart
         | receive '{' ReceiveCases '}'
         | return_keyword MaybeExpression ';'
         | send_keyword Expression to_keyword Expression ';'
         ;

ElsePart: else_keyword '{' Statements '}'
        | elseif_keyword '(' Expression ')' '{' Statements '}' ElsePart
        | %empty
        ;

SwitchCases: SwitchCases SwitchCase
           | %empty
           ;

SwitchCase: case_keyword Literal ':' Statements
          | default_keyword ':' Statements
          ;


ReceiveCases: ReceiveCases ReceiveCase
            | %empty
            ;

ReceiveCase: case_keyword CallMessageIdentifier ':' Statements
           ;

CallMessageIdentifier: identifier
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
          | Expression modulo Expression

          | Expression and_op Expression
          | Expression or_op Expression
          | Expression equal Expression
          | Expression not_equal Expression
          | Expression greater_equal Expression
          | Expression greater_than Expression
          | Expression less_equal Expression
          | Expression less_than Expression

          | '!' Expression
          | bitwise_not Expression
          | Expression bitwise_op Expression
          | Expression bitwise_or Expression
          | Expression bitwise_xor Expression
          | Expression right_shift Expression
          | Expression left_shift Expression
          | '(' Expression ')'
          | identifier '(' ArgsList ')'
          | Location '=' Expression
          | identifier increment
          | identifier decrement
          | Declaration
          | Expression '?' Expression ':' Expression
          | spawn_keyword identifier '(' ArgsList ')'
          ;


Location: identifier
        | identifier '.' Location
        | identifier Indexes
        ;

Indexes: Indexes '[' Expression ']'
       | '[' Expression ']'
       ;

%%

int
yyerror(const char *err) {
    printf("Parse error at line %d: %s\n", yylineno, err);
}
