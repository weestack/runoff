#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

extern int yylineno; /* global variable from flex */

AstNode *append_node(AstNode* siblingA, AstNode* siblingB) {
	AstNode *tmp;
	if(siblingA == NULL)
		return siblingB;

	for (tmp = siblingA; tmp->next != NULL; tmp = tmp->next);

	tmp->next = siblingB;
	tmp->chain = siblingB;
	return siblingA;
}

int nodeLength(AstNode *list){
	int length = 0;
	while(list != NULL){
		list = list->next;
		length++;
	}
	return length;
}


static AstNode *mkNode(int type, AstNode *firstChild) {
	AstNode *node = malloc(sizeof(AstNode));
	node->tag = type;
	node->next = NULL;
	node->children = firstChild;
	node->chain = NULL;
	node->linenum = yylineno;
	return node;
}


/* Adds AstNode to end of chain. */
static AstNode *chain(AstNode *a, AstNode *b){
	AstNode *tmp;

	if(a == b)
		return a;

	if(a == NULL && b != NULL)
		return b;

	/* zoom down to the end of the chain and add b */
	for(tmp = a; tmp->chain != NULL; tmp = tmp->chain);

	tmp->chain = b;
	return a;
}


#define SETCHILD(Tag, A) \
	do{ \
		AstNode *tmp; \
		node->node.Tag.A = A; \
		node->children = chain(node->children, node->node.Tag.A); \
		for(tmp = node->node.Tag.A; tmp != NULL; tmp = tmp->next) \
			tmp->parent = node; \
	}while(0)

AstNode *mkProgNode(AstNode *toplevels){
	AstNode *node = mkNode(Prog, toplevels);
	SETCHILD(Prog,toplevels);
	return node;
}

AstNode *mkDefineFunctionNode(AstNode *identifier, AstNode *parameters, AstNode *type, AstNode *statements){
	AstNode *node = mkNode(DefineFunction, identifier);
	SETCHILD(DefineFunction,identifier);
	SETCHILD(DefineFunction,parameters);
	SETCHILD(DefineFunction,type);
	SETCHILD(DefineFunction,statements);
	return node;
}

AstNode *mkDefineTaskNode(AstNode *identifier, AstNode *parameters, AstNode *statements){
	AstNode *node = mkNode(DefineTask, identifier);
	SETCHILD(DefineTask,identifier);
	SETCHILD(DefineTask,parameters);
	SETCHILD(DefineTask,statements);
	return node;
}

AstNode *mkDefineStructNode(AstNode *identifier, AstNode *fields){
	AstNode *node = mkNode(DefineStruct, identifier);
	SETCHILD(DefineStruct,identifier);
	SETCHILD(DefineStruct,fields);
	return node;
}

AstNode *mkDefineMessageNode(AstNode *messagesIdentifiers){
	AstNode *node = mkNode(DefineMessage, messagesIdentifiers);
	SETCHILD(DefineMessage,messagesIdentifiers);
	return node;
}

AstNode *mkMessageIdentifierNode(AstNode *identifier, AstNode *parameters){
	AstNode *node = mkNode(MessageIdentifier, identifier);
	SETCHILD(MessageIdentifier,identifier);
	SETCHILD(MessageIdentifier,parameters);
	return node;
}

AstNode *mkStructMemberNode(AstNode *identifier, AstNode *type){
	AstNode *node = mkNode(StructMember, identifier);
	SETCHILD(StructMember,identifier);
	SETCHILD(StructMember,type);
	return node;
}

AstNode *mkParameterNode(AstNode *type, AstNode *identifier){
	AstNode *node = mkNode(Parameter, type);
	SETCHILD(Parameter,type);
	SETCHILD(Parameter,identifier);
	return node;
}

AstNode *mkBuiltinTypeNode(int type){
	AstNode *node = mkNode(BuiltinType, NULL);
	node->node.BuiltinType.type = type;
	return node;
}

AstNode *mkStructTypeNode(AstNode *identifier){
	AstNode *node = mkNode(StructType, identifier);
	SETCHILD(StructType,identifier);
	return node;
}

AstNode *mkArrayTypeNode(AstNode *elementType, AstNode *dimensions){
	AstNode *node = mkNode(ArrayType, elementType);
	SETCHILD(ArrayType,elementType);
	SETCHILD(ArrayType,dimensions);
	return node;
}

AstNode *mkWhileNode(AstNode *expression, AstNode *statements){
	AstNode *node = mkNode(While, expression);
	SETCHILD(While,expression);
	SETCHILD(While,statements);
	return node;
}

AstNode *mkForNode(AstNode *expressionInit, AstNode *expressionTest, AstNode *expressionUpdate, AstNode *statements){
	AstNode *node = mkNode(For, expressionInit);
	SETCHILD(For,expressionInit);
	SETCHILD(For,expressionTest);
	SETCHILD(For,expressionUpdate);
	SETCHILD(For,statements);
	return node;
}

AstNode *mkSwitchNode(AstNode *expression, AstNode *cases){
	AstNode *node = mkNode(Switch, expression);
	SETCHILD(Switch,expression);
	SETCHILD(Switch,cases);
	return node;
}

AstNode *mkSwitchCaseNode(AstNode *literal, AstNode *statements){
	AstNode *node = mkNode(SwitchCase, literal);
	SETCHILD(SwitchCase,literal);
	SETCHILD(SwitchCase,statements);
	return node;
}

AstNode *mkIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
	AstNode *node = mkNode(If, expression);
	SETCHILD(If,expression);
	SETCHILD(If,statements);
	SETCHILD(If,elsePart);
	return node;
}

AstNode *mkElseIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
	AstNode *node = mkNode(ElseIf, expression);
	SETCHILD(ElseIf,expression);
	SETCHILD(ElseIf,statements);
	SETCHILD(ElseIf,elsePart);
	return node;
}

AstNode *mkElseNode(AstNode *statements){
	AstNode *node = mkNode(Else, statements);
	SETCHILD(Else,statements);
	return node;
}

AstNode *mkReceiveNode(AstNode *cases){
	AstNode *node = mkNode(Receive, cases);
	SETCHILD(Receive,cases);
	return node;
}

AstNode *mkReceiveCaseNode(AstNode *messageName, AstNode *dataNames, AstNode *statements){
	AstNode *node = mkNode(ReceiveCase, messageName);
	SETCHILD(ReceiveCase,messageName);
	SETCHILD(ReceiveCase,dataNames);
	SETCHILD(ReceiveCase,statements);
	return node;
}

AstNode *mkVarDeclNode(AstNode *type, AstNode *identifier, AstNode *expression, int toplevel){
	AstNode *node = mkNode(VarDecl, type);
	SETCHILD(VarDecl,type);
	SETCHILD(VarDecl,identifier);
	SETCHILD(VarDecl,expression);
	node->node.VarDecl.toplevel = toplevel;
	return node;
}

AstNode *mkBinaryOperationNode(AstNode *expression_left, int operator, AstNode *expression_right){
	AstNode *node = mkNode(BinaryOperation, expression_left);
	SETCHILD(BinaryOperation,expression_left);
	node->node.BinaryOperation.operator = operator;
	SETCHILD(BinaryOperation,expression_right);
	return node;
}

AstNode *mkVariableLocationNode(AstNode *identifier){
	AstNode *node = mkNode(VariableLocation, identifier);
	SETCHILD(VariableLocation,identifier);
	return node;
}

AstNode *mkStructLocationNode(AstNode *identifier, AstNode *location){
	AstNode *node = mkNode(StructLocation, identifier);
	SETCHILD(StructLocation,identifier);
	SETCHILD(StructLocation,location);
	return node;
}

AstNode *mkArrayLocationNode(AstNode *identifier, AstNode *indicies){
	AstNode *node = mkNode(ArrayLocation, identifier);
	SETCHILD(ArrayLocation,identifier);
	SETCHILD(ArrayLocation,indicies);
	return node;
}

AstNode *mkUnaryOperationNode(int operator, int fix, AstNode *expression){
	AstNode *node = mkNode(UnaryOperation, expression);
	node->node.UnaryOperation.operator = operator;
	node->node.UnaryOperation.fix = fix;
	SETCHILD(UnaryOperation,expression);
	return node;
}

AstNode *mkFunctionCallNode(AstNode *identifier, AstNode *arguments){
	AstNode *node = mkNode(FunctionCall, identifier);
	SETCHILD(FunctionCall,identifier);
	SETCHILD(FunctionCall,arguments);
	return node;
}

AstNode *mkAssignmentNode(AstNode *location, AstNode *expression){
	AstNode *node = mkNode(Assignment, location);
	SETCHILD(Assignment,location);
	SETCHILD(Assignment,expression);
	return node;
}

AstNode *mkTernaryOperatorNode(AstNode *expressionTest, AstNode *expressionTrue, AstNode *expressionFalse){
	AstNode *node = mkNode(TernaryOperator, expressionTest);
	SETCHILD(TernaryOperator,expressionTest);
	SETCHILD(TernaryOperator,expressionTrue);
	SETCHILD(TernaryOperator,expressionFalse);
	return node;
}

AstNode *mkIdentifierNode(char *identifier){
	AstNode *node = mkNode(Identifier, NULL);
	node->node.Identifier.identifier = identifier;
	node->node.Identifier.symbol = NULL;
	return node;
}

AstNode *mkPinLiteralNode(long int value){
	AstNode *node = mkNode(PinLiteral, NULL);
	node->node.PinLiteral.value = value;
	return node;
}

AstNode *mkIntLiteralNode(long int value){
	AstNode *node = mkNode(IntLiteral, NULL);
	node->node.IntLiteral.value = value;
	return node;
}

AstNode *mkFloatLiteralNode(float value){
	AstNode *node = mkNode(FloatLiteral, NULL);
	node->node.FloatLiteral.value = value;
	return node;
}

AstNode *mkBoolLiteralNode(int value){
	AstNode *node = mkNode(BoolLiteral, NULL);
	node->node.BoolLiteral.value = value;
	return node;
}

AstNode *mkMessageLiteralNode(AstNode *identifier, AstNode *arguments){
	AstNode *node = mkNode(MessageLiteral, identifier);
	SETCHILD(MessageLiteral,identifier);
	SETCHILD(MessageLiteral,arguments);
	return node;
}

AstNode *mkReturnNode(AstNode *expression){
	AstNode *node = mkNode(Return, expression);
	SETCHILD(Return,expression);
	node->node.Return.functionsym = NULL;
	return node;
}

AstNode *mkSpawnNode(AstNode *identifier, AstNode *arguments){
	static int taskid = 0;
	AstNode *node = mkNode(Spawn, identifier);
	SETCHILD(Spawn,identifier);
	SETCHILD(Spawn,arguments);
	node->node.Spawn.taskId = taskid;
	taskid++;
	return node;
}

AstNode *mkSendNode(AstNode *message, AstNode *receiver){
	AstNode *node = mkNode(Send, message);
	SETCHILD(Send,message);
	SETCHILD(Send,receiver);
	return node;
}

AstNode *mkExprStmtNode(AstNode *expression){
	AstNode *node = mkNode(ExprStmt, expression);
	SETCHILD(ExprStmt,expression);
	return node;
}
