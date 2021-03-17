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
%type <astNode> Program Toplevel MessageIdentifiers MessageIdentifier StructMembers ParametersList Parameters ArgsList Args Type Statements Statement ElsePart SwitchCases SwitchCase ReceiveCases CallMessageIdentifier Identifiers MaybeExpression Declaration Literal Expression Location Indexes

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

Program: Toplevel {$$ = mkProgNode($1);}
       | Program Toplevel {$$ = $1; append_node($1->node.Prog.toplevels, $2);}
       ;

Toplevel: const_keyword identifier int_literal ';' {$$ = mkDefineConstNode($2, $3); }
        | function identifier '(' ParametersList ')' right_arrow Type '{' Statements '}' { $$=mkDefineFunctionNode($2, $4, $7, $9); }
        | task identifier '(' ParametersList ')' '{' Statements '}' {$$ = mkDefineTaskNode($2, $4, $7);}
        | struct_keyword identifier '{' StructMembers '}' {$$=mkDefineStructNode($2, $4);}
        | messages '{' MessageIdentifiers '}' {$$ = mkDefineMessageNode($3);}
        /*| include_keyword '(' identifier ')' ';'*/
        ;

MessageIdentifiers: MessageIdentifiers MessageIdentifier {$$ = $1; append_node($1, $2);}
                  | MessageIdentifier {$$ = $1;}
                  ;

MessageIdentifier: identifier '(' Parameters ')' ';' {$$ = mkMessageIdentifierNode($1, $3);}
                 | identifier ';' {$$ = mkMessageIdentifierNode($1, NULL);}
                 ;

StructMembers: StructMembers Type identifier ';' {$$ = $1; append_node($1, mkStructMemberNode($3, $2));}
             | Type identifier ';' {$$ = mkStructMemberNode($2, $1);}
             ;


ParametersList: Parameters {$$ = $1;}
              | %empty {$$ = NULL;}
              ;

Parameters: Type identifier {$$ = mkParameterNode($1, $2);}
          | Parameters ',' Type identifier {$$ = $1; append_node($1, mkParameterNode($3, $4));}
          ;


ArgsList: Args {$$ = $1;}
        | %empty {$$ = NULL;}
        ;

Args: Args ',' Expression {$$ = $1; append_node($1, $3);}
    | Expression {$$ = $1;}
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


ReceiveCases: ReceiveCases case_keyword CallMessageIdentifier ':' Statements
            | %empty
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
