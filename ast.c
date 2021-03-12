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
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Prog;
        node->next = NULL;
        node->node.ProgNode.toplevels = toplevels;
        return node;
}

AstNode *mkDefineConstNode(AstNode *identifier, AstNode *int_literal){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineConst;
        node->next = NULL;
        node->node.DefineConstNode.identifier = identifier;
        node->node.DefineConstNode.int_literal = int_literal;
        return node;
}

AstNode *mkDefineFunctionNode(AstNode *identifier, AstNode *parameters, AstNode *type, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineFunction;
        node->next = NULL;
        node->node.DefineFunctionNode.identifier = identifier;
        node->node.DefineFunctionNode.parameters = parameters;
        node->node.DefineFunctionNode.type = type;
        node->node.DefineFunctionNode.statement = statement;
        return node;
}

AstNode *mkDefineTaskNode(AstNode *identifier, AstNode *parameters, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineTask;
        node->next = NULL;
        return node;
}

AstNode *mkDefineMessageNode(AstNode *messagesIdentifiers){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineMessage;
        node->next = NULL;
        return node;
}

AstNode *mkIncludeRunoffFileNode(AstNode *identifier){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = IncludeRunoffFile;
        node->next = NULL;
        return node;
}

AstNode *mkMessageIdentifierNode(AstNode *identifier, AstNode *parameters){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = MessageIdentifier;
        node->next = NULL;
        return node;
}

AstNode *mkStructMemberNode(AstNode *identifier, AstNode *type){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructMember;
        node->next = NULL;
        return node;
}

AstNode *mkParameterNode(AstNode *type, AstNode *identifier){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Parameter;
        node->next = NULL;
        return node;
}

AstNode *mkStructTypeNode(AstNode *identifier){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructType;
        node->next = NULL;
        return node;
}

AstNode *mkArrayTypeNode(AstNode *type, AstNode *int_literal){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ArrayType;
        node->next = NULL;
        return node;
}

AstNode *mkWhileNode(AstNode *expression, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = While;
        node->next = NULL;
        return node;
}

AstNode *mkForNode(AstNode *expressionInit, AstNode *expressionTest, AstNode *expressionUpdate, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = For;
        node->next = NULL;
        return node;
}

AstNode *mkSwitchNode(AstNode *expression, AstNode *cases){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Switch;
        node->next = NULL;
        return node;
}

AstNode *mkSwitchCaseNode(AstNode *literal, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = SwitchCase;
        node->next = NULL;
        return node;
}

AstNode *mkIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = If;
        node->next = NULL;
        return node;
}

AstNode *mkElseIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ElseIf;
        node->next = NULL;
        return node;
}

AstNode *mkElseNode(AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Else;
        node->next = NULL;
        return node;
}

AstNode *mkReceiveNode(AstNode *cases){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Receive;
        node->next = NULL;
        return node;
}


AstNode *mkReceiveCaseNode(AstNode *messageName, AstNode *dataNames, AstNode *statements){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ReceiveCase;
        node->next = NULL;
        return node;
}


AstNode *mkVarDeclNode(AstNode *identifier, AstNode *expression){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = VarDecl;
        node->next = NULL;
        return node;
}


AstNode *mkBinaryOperationNode(AstNode *expression_left, int operator, AstNode *expression_right){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = BinaryOperation;
        node->next = NULL;
        return node;
}


AstNode *mkVariableLocationNode(AstNode *identifier){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = VariableLocation;
        node->next = NULL;
        return node;
}


AstNode *mkStructLocationNode(AstNode *identifier, AstNode *location){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructLocation;
        node->next = NULL;
        return node;
}


AstNode *mkArrayLocationNode(AstNode *identifier, AstNode *indicies){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ArrayLocation;
        node->next = NULL;
        return node;
}


AstNode *mkUnaryOperationNode(int operator, AstNode *expression){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = UnaryOperation;
        node->next = NULL;
        return node;
}


AstNode *mkFunctionCallNode(AstNode *identifier, AstNode *arguments){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = FunctionCall;
        node->next = NULL;
        return node;
}


AstNode *mkAssignmentNode(AstNode *location, AstNode *expression){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Assignment;
        node->next = NULL;
        return node;
}


AstNode *mkTernaryOperatorNode(AstNode *expressionTest, AstNode *expressionTrue, AstNode *expressionFalse){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = TernaryOperator;
        node->next = NULL;
        return node;
}
