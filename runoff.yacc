%{
    #include <stdio.h>
    #include "ast.h"
    #include "phases.h"

    extern int yylineno;
    extern int yylex(void);
    extern FILE* yyin;
    int yyerror(const char*);
    static AstNode *parseresult;
%}

/* Give me the good errors please */
%define parse.error verbose

%union {
    struct AstNode* astNode;
}

/* Define all non-terminals as type astNode */
%type <astNode> Program Toplevel MessageIdentifiers MessageIdentifier StructMembers ParametersList Parameters ArgsList Args Type Statements Statement ElsePart SwitchCases SwitchCase ReceiveCases Identifiers MaybeExpression Declaration Literal Expression Location Indexes

%token <astNode> identifier builtin_type int_literal float_literal bool_literal

%token
    const_keyword
    struct_keyword
    pinid_keyword
    messages

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
    right_arrow
    return_keyword
    spawn_keyword
    send_keyword
    to_keyword

    include_keyword

    bitwise_and
    bitwise_or
    bitwise_xor
    bitwise_not
    right_shift
    left_shift

%right '='
%right '?' ':'
%left or_op
%left and_op
%left bitwise_or
%left bitwise_xor
%left bitwise_and
%left equal not_equal
%left greater_than greater_equal less_than less_equal
%left right_shift left_shift
%left '+' '-'
%left '*' '/' modulo
%right '!' bitwise_not
%left increment decrement
%left '(' ')' '[' ']' '.'

%%

Program: Toplevel {$$ = mkProgNode($1); parseresult = $$;}
       | Program Toplevel {
       		$$ = append_node($1->node.Prog.toplevels, $2);
       		$$ = mkProgNode($$);
       		parseresult = $$;
       		}
       ;

Toplevel: const_keyword identifier int_literal ';' {$$ = mkDefineConstNode($2, $3); }
        | function identifier '(' ParametersList ')' right_arrow Type '{' Statements '}' { $$=mkDefineFunctionNode($2, $4, $7, $9); }
        | task identifier '(' ParametersList ')' '{' Statements '}' {$$ = mkDefineTaskNode($2, $4, $7);}
        | struct_keyword identifier '{' StructMembers '}' {$$=mkDefineStructNode($2, $4);}
        | messages '{' MessageIdentifiers '}' {$$ = mkDefineMessageNode($3);}
        | pinid_keyword identifier '=' int_literal ';'
        /*| include_keyword '(' identifier ')' ';'*/
        ;

MessageIdentifiers: MessageIdentifiers MessageIdentifier {$$ = append_node($1, $2);}
                  | MessageIdentifier {$$ = $1;}
                  ;

MessageIdentifier: identifier '(' Parameters ')' ';' {$$ = mkMessageIdentifierNode($1, $3);}
                 | identifier ';' {$$ = mkMessageIdentifierNode($1, NULL);}
                 ;

StructMembers: StructMembers Type identifier ';' {$$ = append_node($1, mkStructMemberNode($3, $2));}
             | Type identifier ';' {$$ = mkStructMemberNode($2, $1);}
             ;


ParametersList: Parameters {$$ = $1;}
              | %empty {$$ = NULL;}
              ;

Parameters: Type identifier {$$ = mkParameterNode($1, $2);}
          | Parameters ',' Type identifier {$$ = append_node($1, mkParameterNode($3, $4));}
          ;


ArgsList: Args {$$ = $1;}
        | %empty {$$ = NULL;}
        ;

Args: Args ',' Expression {$$ = append_node($1, $3);}
    | Expression {$$ = $1;}
    ;

Type: builtin_type {$$ = $1;}
    | struct_keyword identifier {$$ = mkStructTypeNode($2);}
    | Type '[' int_literal ']' {$$ = mkArrayTypeNode($1, $3);}
    | Type '[' ']' {$$ = mkArrayTypeNode($1, NULL); }
    ;

Statements: Statements Statement {$$ = append_node($1, $2);}
          | %empty {$$ = NULL;}
          ;

Statement: Expression ';' {$$ = mkExprStmtNode($1);}
         | while_keyword '(' Expression ')' '{' Statements '}' {$$ = mkWhileNode($3, $6);}
         | for_keyword '(' MaybeExpression ';' MaybeExpression ';' MaybeExpression ')' '{' Statements '}'
         {$$ = mkForNode($3, $5, $7, $10);}
         | switch_keyword '(' Expression ')' '{' SwitchCases '}' {$$ = mkSwitchNode($3, $6);}
         | if_keyword '(' Expression ')' '{' Statements '}' ElsePart {$$ = mkIfNode($3, $6, $8);}
         | receive '{' ReceiveCases '}' {$$ = mkReceiveNode($3);}
         | return_keyword MaybeExpression ';' {$$ = mkReturnNode($2);}
         | send_keyword Expression to_keyword Expression ';' {$$ = mkSendNode($2, $4);}
         ;

ElsePart: else_keyword '{' Statements '}' {$$ = mkElseNode($3);}
        | elseif_keyword '(' Expression ')' '{' Statements '}' ElsePart {$$ = mkElseIfNode($3, $6, $8);}
        | %empty {$$ = NULL;}
        ;

SwitchCases: SwitchCases SwitchCase {$$ = append_node($1, $2);}
           | %empty {$$ = NULL;}
           ;

SwitchCase: case_keyword Literal ':' Statements {$$ = mkSwitchCaseNode($2, $4);}
          | default_keyword ':' Statements {$$ = mkSwitchCaseNode(NULL, $3);}
          ;


ReceiveCases: ReceiveCases case_keyword identifier ':' Statements {$$ = append_node($1, mkReceiveCaseNode($3, NULL, $5));}
            | ReceiveCases case_keyword identifier '(' Identifiers ')' ':' Statements {$$ = append_node($1, mkReceiveCaseNode($3, $5, $8));}
            | %empty {$$ = NULL;}
            ;

Identifiers: Identifiers ',' identifier {$$ = append_node($1, $3);}
           | identifier {$$ = $1;}
           ;

MaybeExpression: Expression {$$ = $1;}
               | %empty {$$ = NULL;}
               ;

Declaration: Type identifier {$$ = mkVarDeclNode($1, $2, NULL);}
           | Type identifier '=' Expression {$$ = mkVarDeclNode($1, $2, $4);}
           ;

Literal: int_literal {$$ = $1;}
       | float_literal {$$ = $1;}
       | bool_literal {$$ = $1;}
       ;



Expression: Location {$$ = $1;}
          | Literal {$$ = $1;}
          | Expression '+' Expression {$$ = mkBinaryOperationNode($1, eplus, $3);}
          | Expression '-' Expression {$$ = mkBinaryOperationNode($1, eminus, $3);}
          | Expression '*' Expression {$$ = mkBinaryOperationNode($1, etimes, $3);}
          | Expression '/' Expression {$$ = mkBinaryOperationNode($1, edivid, $3);}
          | Expression modulo Expression {$$ = mkBinaryOperationNode($1, emod, $3);}

          | Expression and_op Expression {$$ = mkBinaryOperationNode($1, elogical_and, $3);}
          | Expression or_op Expression {$$ = mkBinaryOperationNode($1, elogical_or, $3);}
          | Expression equal Expression {$$ = mkBinaryOperationNode($1, eequal, $3);}
          | Expression not_equal Expression {$$ = mkBinaryOperationNode($1, enot_equal, $3);}
          | Expression greater_equal Expression {$$ = mkBinaryOperationNode($1, egreater_equal, $3);}
          | Expression greater_than Expression {$$ = mkBinaryOperationNode($1, ebigger_than, $3);}
          | Expression less_equal Expression {$$ = mkBinaryOperationNode($1, esmaller_equal, $3);}
          | Expression less_than Expression {$$ = mkBinaryOperationNode($1, esmaller_than, $3);}

          | '!' Expression {$$ = mkUnaryOperationNode(elogical_not, prefix, $2);}
          | bitwise_not Expression {$$ = mkUnaryOperationNode(ebit_not, prefix, $2);}
          | Expression bitwise_and Expression {$$ = mkBinaryOperationNode($1, ebit_and, $3);}
          | Expression bitwise_or Expression {$$ = mkBinaryOperationNode($1, ebit_or, $3);}
          | Expression bitwise_xor Expression {$$ = mkBinaryOperationNode($1, ebit_xor, $3);}
          | Expression right_shift Expression {$$ = mkBinaryOperationNode($1, eright_shift, $3);}
          | Expression left_shift Expression {$$ = mkBinaryOperationNode($1, eleft_shift, $3);}
          | '(' Expression ')' {$$ = $2;}
          | identifier '(' ArgsList ')' {$$ = mkFunctionCallNode($1, $3);}
          | Location '=' Expression {$$ = mkAssignmentNode($1, $3);}
          | identifier increment {$$ = mkUnaryOperationNode(eincrement, postfix, $1);}
          | identifier decrement {$$ = mkUnaryOperationNode(edecrement, postfix, $1);}
          | Declaration {$$ = $1;}
          | Expression '?' Expression ':' Expression {$$ = mkTernaryOperatorNode($1, $3, $5);}
          | spawn_keyword identifier '(' ArgsList ')' {$$ = mkSpawnNode($2, $4);}
          ;


Location: identifier {$$ = mkVariableLocationNode($1);}
        | identifier '.' Location {$$ = mkStructLocationNode($1, $3);}
        | identifier Indexes {$$ = mkArrayLocationNode($1, $2);}
        ;

Indexes: Indexes '[' Expression ']' {$$ = append_node($1, $3);}
       | '[' Expression ']' {$$ = $2;}
       ;

%%

int
yyerror(const char *err) {
    printf("Parse error at line %d: %s\n", yylineno, err);
}

AstNode *parse(char *file){
	int parseok;

	yyin = fopen(file, "r");
	if(yyin == NULL)
		return NULL;

	parseok = yyparse();
	if(parseok == 0)
		return parseresult;
	else
		return NULL;
}
