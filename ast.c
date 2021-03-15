#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

static AstNode *mkNode(int type) {
	AstNode *node = malloc(sizeof(AstNode));
	node->tag = type;
	node->next = NULL;
	return node;
}

AstNode *mkProgNode(AstNode *toplevels){
	AstNode *node = mkNode(Prog);
	node->node.Prog.toplevels = toplevels;
	return node;
}

AstNode *mkDefineConstNode(AstNode *identifier, AstNode *int_literal){
	AstNode *node = mkNode(DefineConst);
	node->node.DefineConst.identifier = identifier;
	node->node.DefineConst.int_literal = int_literal;
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

AstNode *mkVarDeclNode(AstNode *type, AstNode *identifier, AstNode *expression){
	AstNode *node = mkNode(VarDecl);
	node->node.VarDecl.type = type;
	node->node.VarDecl.identifier = identifier;
	node->node.VarDecl.expression = expression;
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
	return node;
}