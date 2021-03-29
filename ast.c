#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"

extern int yylineno; /* global variable from flex */

AstNode *append_node(AstNode* siblingA, AstNode* siblingB) {
	AstNode *tmp;
	if(siblingA == NULL)
		return siblingB;

	for (tmp = siblingA; tmp->next != NULL; tmp = tmp->next);

	tmp->next = siblingB;

	return siblingA;
}

AstNode *concat_node(AstNode *siblingA, AstNode* siblingB){
	AstNode *tmp;
	AstNode *new_list;
	AstNode *start;

	if(siblingA == NULL)
		return siblingB;
	if(siblingB == NULL)
		return siblingA;

	tmp=siblingA;
	start = malloc(sizeof(AstNode));
	memcpy(start, tmp, sizeof(AstNode));
	start->next = NULL;

	for(tmp=tmp->next; tmp != NULL; tmp=tmp->next){
		new_list = malloc(sizeof(AstNode));
		memcpy(new_list, tmp, sizeof(AstNode));
		new_list->next = NULL;
		append_node(start, new_list);
	}

	for(tmp=siblingB; tmp != NULL; tmp=tmp->next){
		new_list = malloc(sizeof(AstNode));
		memcpy(new_list, tmp, sizeof(AstNode));
		new_list->next = NULL;
		append_node(start, new_list);
	}

	return start;
}

int nodeLength(AstNode *list){
	int length = 0;
	while(list != NULL){
		list = list->next;
		length++;
	}
	return length;
}


static AstNode *mkNode(int type) {
	AstNode *node = malloc(sizeof(AstNode));
	node->tag = type;
	node->next = NULL;
	node->linenum = yylineno;
	return node;
}

AstNode *mkProgNode(AstNode *toplevels){
	AstNode *node = mkNode(Prog);
	node->node.Prog.toplevels = toplevels;
	return node;
}

AstNode *mkDefineFunctionNode(AstNode *identifier, AstNode *parameters, AstNode *type, AstNode *statements){
	AstNode *node = mkNode(DefineFunction);
	node->node.DefineFunction.identifier = identifier;
	node->node.DefineFunction.parameters = parameters;
	node->node.DefineFunction.type = type;
	node->node.DefineFunction.statements = statements;
	return node;
}

AstNode *mkDefineTaskNode(AstNode *identifier, AstNode *parameters, AstNode *statements){
	AstNode *node = mkNode(DefineTask);
	node->node.DefineTask.identifier = identifier;
	node->node.DefineTask.parameters = parameters;
	node->node.DefineTask.statements = statements;
	return node;
}

AstNode *mkDefineStructNode(AstNode *identifier, AstNode *fields){
	AstNode *node = mkNode(DefineStruct);
	node->node.DefineStruct.identifier = identifier;
	node->node.DefineStruct.fields = fields;
	return node;
}

AstNode *mkDefineMessageNode(AstNode *messagesIdentifiers){
	AstNode *node = mkNode(DefineMessage);
	node->node.DefineMessage.messagesIdentifiers = messagesIdentifiers;
	return node;
}

AstNode *mkIncludeRunoffFileNode(AstNode *identifier){
	AstNode *node = mkNode(IncludeRunoffFile);
	node->node.IncludeRunoffFile.identifier = identifier;
	return node;
}

AstNode *mkMessageIdentifierNode(AstNode *identifier, AstNode *parameters){
	AstNode *node = mkNode(MessageIdentifier);
	node->node.MessageIdentifier.identifier = identifier;
	node->node.MessageIdentifier.parameters = parameters;
	return node;
}

AstNode *mkStructMemberNode(AstNode *identifier, AstNode *type){
	AstNode *node = mkNode(StructMember);
	node->node.StructMember.identifier = identifier;
	node->node.StructMember.type = type;
	return node;
}

AstNode *mkParameterNode(AstNode *type, AstNode *identifier){
	AstNode *node = mkNode(Parameter);
	node->node.Parameter.type = type;
	node->node.Parameter.identifier = identifier;
	return node;
}

AstNode *mkBuiltinTypeNode(int type){
	AstNode *node = mkNode(BuiltinType);
	node->node.BuiltinType.type = type;
	return node;
}

AstNode *mkStructTypeNode(AstNode *identifier){
	AstNode *node = mkNode(StructType);
	node->node.StructType.identifier = identifier;
	return node;
}

AstNode *mkArrayTypeNode(AstNode *type, AstNode *int_literal){
	AstNode *node = mkNode(ArrayType);
	node->node.ArrayType.type = type;
	node->node.ArrayType.int_literal = int_literal;
	return node;
}

AstNode *mkWhileNode(AstNode *expression, AstNode *statements){
	AstNode *node = mkNode(While);
	node->node.While.expression = expression;
	node->node.While.statements = statements;
	return node;
}

AstNode *mkForNode(AstNode *expressionInit, AstNode *expressionTest, AstNode *expressionUpdate, AstNode *statements){
	AstNode *node = mkNode(For);
	node->node.For.expressionInit = expressionInit;
	node->node.For.expressionTest = expressionTest;
	node->node.For.expressionUpdate = expressionUpdate;
	node->node.For.statements = statements;
	return node;
}

AstNode *mkSwitchNode(AstNode *expression, AstNode *cases){
	AstNode *node = mkNode(Switch);
	node->node.Switch.expression = expression;
	node->node.Switch.cases = cases;
	return node;
}

AstNode *mkSwitchCaseNode(AstNode *literal, AstNode *statements){
	AstNode *node = mkNode(SwitchCase);
	node->node.SwitchCase.literal = literal;
	node->node.SwitchCase.statements = statements;
	return node;
}

AstNode *mkIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
	AstNode *node = mkNode(If);
	node->node.If.expression = expression;
	node->node.If.statements = statements;
	node->node.If.elsePart = elsePart;
	return node;
}

AstNode *mkElseIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
	AstNode *node = mkNode(ElseIf);
	node->node.ElseIf.expression = expression;
	node->node.ElseIf.statements = statements;
	node->node.ElseIf.elsePart = elsePart;
	return node;
}

AstNode *mkElseNode(AstNode *statements){
	AstNode *node = mkNode(Else);
	node->node.Else.statements = statements;
	return node;
}

AstNode *mkReceiveNode(AstNode *cases){
	AstNode *node = mkNode(Receive);
	node->node.Receive.cases = cases;
	return node;
}

AstNode *mkReceiveCaseNode(AstNode *messageName, AstNode *dataNames, AstNode *statements){
	AstNode *node = mkNode(ReceiveCase);
	node->node.ReceiveCase.messageName = messageName;
	node->node.ReceiveCase.dataNames = dataNames;
	node->node.ReceiveCase.statements = statements;
	return node;
}

AstNode *mkVarDeclNode(AstNode *type, AstNode *identifier, AstNode *expression, int toplevel){
	AstNode *node = mkNode(VarDecl);
	node->node.VarDecl.type = type;
	node->node.VarDecl.identifier = identifier;
	node->node.VarDecl.expression = expression;
	node->node.VarDecl.toplevel = toplevel;
	return node;
}

AstNode *mkBinaryOperationNode(AstNode *expression_left, int operator, AstNode *expression_right){
	AstNode *node = mkNode(BinaryOperation);
	node->node.BinaryOperation.expression_left = expression_left;
	node->node.BinaryOperation.operator = operator;
	node->node.BinaryOperation.expression_right = expression_right;
	return node;
}

AstNode *mkVariableLocationNode(AstNode *identifier){
	AstNode *node = mkNode(VariableLocation);
	node->node.VariableLocation.identifier = identifier;
	return node;
}

AstNode *mkStructLocationNode(AstNode *identifier, AstNode *location){
	AstNode *node = mkNode(StructLocation);
	node->node.StructLocation.identifier = identifier;
	node->node.StructLocation.location = location;
	return node;
}

AstNode *mkArrayLocationNode(AstNode *identifier, AstNode *indicies){
	AstNode *node = mkNode(ArrayLocation);
	node->node.ArrayLocation.identifier = identifier;
	node->node.ArrayLocation.indicies = indicies;
	return node;
}

AstNode *mkUnaryOperationNode(int operator, int fix, AstNode *expression){
	AstNode *node = mkNode(UnaryOperation);
	node->node.UnaryOperation.operator = operator;
	node->node.UnaryOperation.fix = fix;
	node->node.UnaryOperation.expression = expression;
	return node;
}

AstNode *mkFunctionCallNode(AstNode *identifier, AstNode *arguments){
	AstNode *node = mkNode(FunctionCall);
	node->node.FunctionCall.identifier = identifier;
	node->node.FunctionCall.arguments = arguments;
	return node;
}

AstNode *mkAssignmentNode(AstNode *location, AstNode *expression){
	AstNode *node = mkNode(Assignment);
	node->node.Assignment.location = location;
	node->node.Assignment.expression = expression;
	return node;
}

AstNode *mkTernaryOperatorNode(AstNode *expressionTest, AstNode *expressionTrue, AstNode *expressionFalse){
	AstNode *node = mkNode(TernaryOperator);
	node->node.TernaryOperator.expressionTest = expressionTest;
	node->node.TernaryOperator.expressionTrue = expressionTrue;
	node->node.TernaryOperator.expressionFalse = expressionFalse;
	return node;
}

AstNode *mkIdentifierNode(char *identifier){
	AstNode *node = mkNode(Identifier);
	node->node.Identifier.identifier = identifier;
	node->node.Identifier.symbol = NULL;
	return node;
}

AstNode *mkPinLiteralNode(long int value){
	AstNode *node = mkNode(PinLiteral);
	node->node.PinLiteral.value = value;
	return node;
}

AstNode *mkIntLiteralNode(long int value){
	AstNode *node = mkNode(IntLiteral);
	node->node.IntLiteral.value = value;
	return node;
}

AstNode *mkFloatLiteralNode(float value){
	AstNode *node = mkNode(FloatLiteral);
	node->node.FloatLiteral.value = value;
	return node;
}

AstNode *mkBoolLiteralNode(int value){
	AstNode *node = mkNode(BoolLiteral);
	node->node.BoolLiteral.value = value;
	return node;
}

AstNode *mkMessageLiteralNode(AstNode *identifier, AstNode *arguments){
	AstNode *node = mkNode(MessageLiteral);
	node->node.MessageLiteral.identifier = identifier;
	node->node.MessageLiteral.arguments = arguments;
	return node;
}

AstNode *mkReturnNode(AstNode *expression){
	AstNode *node = mkNode(Return);
	node->node.Return.expression = expression;
	node->node.Return.functionsym = malloc(sizeof(Symbol*));
	*node->node.Return.functionsym = NULL;
	return node;
}

AstNode *mkSpawnNode(AstNode *identifier, AstNode *arguments){
	AstNode *node = mkNode(Spawn);
	node->node.Spawn.identifier = identifier;
	node->node.Spawn.arguments = arguments;
	return node;
}

AstNode *mkSendNode(AstNode *message, AstNode *receiver){
	AstNode *node = mkNode(Send);
	node->node.Send.message = message;
	node->node.Send.receiver = receiver;
	return node;
}

AstNode *mkExprStmtNode(AstNode *expression){
	AstNode *node = mkNode(ExprStmt);
	node->node.ExprStmt.expression = expression;
	return node;
}

AstNode *getChildren(AstNode *node){
	AstNode *result = NULL;

	switch(node->tag){
	case Prog:
		result = node->node.Prog.toplevels;
		break;
	case DefineFunction:
		result = concat_node(node->node.DefineFunction.identifier, node->node.DefineFunction.parameters);
		result = concat_node(result, node->node.DefineFunction.type);
		result = concat_node(result, node->node.DefineFunction.statements);
		break;
	case DefineTask:
		result = concat_node(node->node.DefineTask.identifier, node->node.DefineTask.parameters);
		result = concat_node(result, node->node.DefineTask.statements);
		break;
	case DefineStruct:
		result = concat_node(node->node.DefineStruct.identifier, node->node.DefineStruct.fields);
		break;
	case DefineMessage:
		result = node->node.DefineMessage.messagesIdentifiers;
		break;
	case IncludeRunoffFile:
		result = node->node.IncludeRunoffFile.identifier;
		break;
	case MessageIdentifier:
		result = concat_node(node->node.MessageIdentifier.identifier, node->node.MessageIdentifier.parameters);
		break;
	case StructMember:
		result = concat_node(node->node.StructMember.identifier, node->node.StructMember.type);
		break;
	case Parameter:
		result = concat_node(node->node.Parameter.type, node->node.Parameter.identifier);
		break;
	case BuiltinType:
		break;
	case StructType:
		result = node->node.StructType.identifier;
		break;
	case ArrayType:
		result = concat_node(node->node.ArrayType.type, node->node.ArrayType.int_literal);
		break;
	case While:
		result = concat_node(node->node.While.expression, node->node.While.statements);
		break;
	case For:
		result = concat_node(node->node.For.expressionInit, node->node.For.expressionTest);
		result = concat_node(result, node->node.For.expressionUpdate);
		result = concat_node(result, node->node.For.statements);
		break;
	case Switch:
		result = concat_node(node->node.Switch.expression, node->node.Switch.cases);
		break;
	case SwitchCase:
		result = concat_node(node->node.SwitchCase.literal, node->node.SwitchCase.statements);
		break;
	case If:
		result = concat_node(node->node.If.expression, node->node.If.statements);
		result = concat_node(result, node->node.If.elsePart);
		break;
	case ElseIf:
		result = concat_node(node->node.ElseIf.expression, node->node.ElseIf.statements);
		result = concat_node(result, node->node.ElseIf.elsePart);
		break;
	case Else:
		result = node->node.Else.statements;
		break;
	case Receive:
		result = node->node.Receive.cases;
		break;
	case ReceiveCase:
		result = concat_node(node->node.ReceiveCase.messageName, node->node.ReceiveCase.dataNames);
		result = concat_node(result, node->node.ReceiveCase.statements);
		break;
	case VarDecl:
		result = concat_node(node->node.VarDecl.type, node->node.VarDecl.identifier);
		result = concat_node(result, node->node.VarDecl.expression);
		break;
	case BinaryOperation:
		result = concat_node(node->node.BinaryOperation.expression_left, node->node.BinaryOperation.expression_right);
		break;
	case VariableLocation:
		result = node->node.VariableLocation.identifier;
		break;
	case StructLocation:
		result = concat_node(node->node.StructLocation.identifier, node->node.StructLocation.location);
		break;
	case ArrayLocation:
		result = concat_node(node->node.ArrayLocation.identifier, node->node.ArrayLocation.indicies);
		break;
	case UnaryOperation:
		result = node->node.UnaryOperation.expression;
		break;
	case FunctionCall:
		result = concat_node(node->node.FunctionCall.identifier, node->node.FunctionCall.arguments);
		break;
	case Assignment:
		result = concat_node(node->node.Assignment.location, node->node.Assignment.expression);
		break;
	case TernaryOperator:
		result = concat_node(node->node.TernaryOperator.expressionTest, node->node.TernaryOperator.expressionTrue);
		result = concat_node(result, node->node.TernaryOperator.expressionFalse);
		break;
	case Identifier:
		break;
	case IntLiteral:
		break;
	case FloatLiteral:
		break;
	case BoolLiteral:
		break;
	case MessageLiteral:
		result = concat_node(node->node.MessageLiteral.identifier, node->node.MessageLiteral.arguments);
		break;
	case Return:
		result = node->node.Return.expression;
		break;
	case Spawn:
		result = concat_node(node->node.Spawn.identifier, node->node.Spawn.arguments);
		break;
	case Send:
		result = concat_node(node->node.Send.message, node->node.Send.receiver);
		break;
	case ExprStmt:
		result = node->node.ExprStmt.expression;
		break;
	}

	return result;
}
