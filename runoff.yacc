%{
	#include <stdio.h>
	#include "data.h"
	#include "auxiliary.h"

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
%type <astNode> Program Toplevel MessageIdentifiers MessageIdentifier StructMembers ParametersList Parameters ArgsList Args Type Statements Statement ElsePart SwitchCases SwitchCase ReceiveCases ReceiveCase MaybeExpression Declaration Literal Expression Location Indexes Identifiers IdentifierList DataType ArrayDimensions

%token <astNode> identifier builtin_type pin_literal int_literal float_literal bool_literal

%token
	struct_keyword
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

Toplevel: function identifier '(' ParametersList ')' right_arrow DataType '{' Statements '}' { $$=mkDefineFunctionNode($2, $4, $7, $9); }
	| task identifier '(' ParametersList ')' '{' Statements '}' {$$ = mkDefineTaskNode($2, $4, $7);}
	| struct_keyword identifier '{' StructMembers '}' {$$=mkDefineStructNode($2, $4);}
	| messages '{' MessageIdentifiers '}' {$$ = mkDefineMessageNode($3);}
	| Type identifier '=' Expression ';' {$$ = mkVarDeclNode($1, $2, $4, 1);}
	;

MessageIdentifiers: MessageIdentifiers MessageIdentifier {$$ = append_node($1, $2);}
	| MessageIdentifier {$$ = $1;}
	;

MessageIdentifier: identifier '{' Parameters '}' ';' {$$ = mkMessageIdentifierNode($1, $3);}
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

ArrayDimensions: '[' int_literal ']' ArrayDimensions {$$ = append_node($2, $4);}
	| '[' int_literal ']' {$$ = $2;}
	;

DataType: builtin_type {$$ = $1;}
	| struct_keyword identifier {$$ = mkStructTypeNode($2);}

Type: DataType {$$ = $1;}
	| DataType ArrayDimensions {$$ = mkArrayTypeNode($1, $2);}
	;

Statements: Statements Statement {$$ = append_node($1, $2);}
	| %empty {$$ = NULL;}
	;

Statement: Expression ';' {$$ = mkExprStmtNode($1);}
	| while_keyword '(' Expression ')' '{' Statements '}' {$$ = mkWhileNode($3, $6);}
	| for_keyword '(' MaybeExpression ';' MaybeExpression ';' MaybeExpression ')' '{' Statements '}' {
		$$ = mkForNode($3, $5, $7, $10);
	}
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

SwitchCase: case_keyword int_literal ':' Statements {$$ = mkSwitchCaseNode($2, $4);}
	| default_keyword ':' Statements {$$ = mkSwitchCaseNode(NULL, $3);}
	;

IdentifierList: Identifiers {$$ = $1;}
	| %empty {$$ = NULL;}
	;

Identifiers: Identifiers ',' identifier {$$ = append_node($1, $3);}
	| identifier {$$ = $1;}
	;

ReceiveCases: ReceiveCases ReceiveCase {$$ = append_node($1, $2);}
	| %empty {$$ = NULL;}
	;

ReceiveCase: case_keyword identifier '{' IdentifierList '}' ':' Statements {$$ = mkReceiveCaseNode($2, $4, $7);}
	| default_keyword ':' Statements {$$ = mkReceiveCaseNode(NULL, NULL, $3);}
	;

MaybeExpression: Expression {$$ = $1;}
	| %empty {$$ = NULL;}
	;

Declaration: Type identifier {$$ = mkVarDeclNode($1, $2, NULL, 0);}
	| Type identifier '=' Expression {$$ = mkVarDeclNode($1, $2, $4, 0);}
	;

Literal: pin_literal {$$ = $1;}
	| int_literal {$$ = $1;}
	| float_literal {$$ = $1;}
	| bool_literal {$$ = $1;}
	| identifier '{' ArgsList '}' {$$ = mkMessageLiteralNode($1, $3);}
	;

Expression: Location {$$ = $1;}
	| Literal {$$ = $1;}
	| Expression '+' Expression {$$ = mkBinaryOperationNode($1, OpPlus, $3);}
	| Expression '-' Expression {$$ = mkBinaryOperationNode($1, OpMinus, $3);}
	| Expression '*' Expression {$$ = mkBinaryOperationNode($1, OpTimes, $3);}
	| Expression '/' Expression {$$ = mkBinaryOperationNode($1, OpDivide, $3);}
	| Expression modulo Expression {$$ = mkBinaryOperationNode($1, OpMod, $3);}
	| Expression and_op Expression {$$ = mkBinaryOperationNode($1, OpLogAnd, $3);}
	| Expression or_op Expression {$$ = mkBinaryOperationNode($1, OpLogOr, $3);}
	| Expression equal Expression {$$ = mkBinaryOperationNode($1, OpEqual, $3);}
	| Expression not_equal Expression {$$ = mkBinaryOperationNode($1, OpNotEqual, $3);}
	| Expression greater_equal Expression {$$ = mkBinaryOperationNode($1, OpGreaterEqual, $3);}
	| Expression greater_than Expression {$$ = mkBinaryOperationNode($1, OpGreaterThan, $3);}
	| Expression less_equal Expression {$$ = mkBinaryOperationNode($1, OpSmallerThan, $3);}
	| Expression less_than Expression {$$ = mkBinaryOperationNode($1, OpSmallerThan, $3);}
	| '!' Expression {$$ = mkUnaryOperationNode(OpLogNot, Prefix, $2);}
	| bitwise_not Expression {$$ = mkUnaryOperationNode(OpBitNot, Prefix, $2);}
	| Expression bitwise_and Expression {$$ = mkBinaryOperationNode($1, OpBitAnd, $3);}
	| Expression bitwise_or Expression {$$ = mkBinaryOperationNode($1, OpBitOr, $3);}
	| Expression bitwise_xor Expression {$$ = mkBinaryOperationNode($1, OpBitXor, $3);}
	| Expression right_shift Expression {$$ = mkBinaryOperationNode($1, OpShiftRight, $3);}
	| Expression left_shift Expression {$$ = mkBinaryOperationNode($1, OpShiftLeft, $3);}
	| '(' Expression ')' {$$ = $2;}
	| identifier '(' ArgsList ')' {$$ = mkFunctionCallNode($1, $3);}
	| Location '=' Expression {$$ = mkAssignmentNode($1, $3);}
	| Location increment {$$ = mkUnaryOperationNode(OpIncrement, Postfix, $1);}
	| Location decrement {$$ = mkUnaryOperationNode(OpDecrement, Postfix, $1);}
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
	eprintf(yylineno, "%s\n", err);
	return 0;
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
