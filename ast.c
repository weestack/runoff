#include <stdio.h>
#include <stdlib.h>
#include "ast.h"



AstNode *mkProgNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Prog;
        node->next = NULL;
        return node;
}


AstNode *mkDefineConstNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineConst;
        node->next = NULL;
        return node;
}


AstNode *mkDefineFunctionNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineFunction;
        node->next = NULL;
        return node;
}


AstNode *mkDefineTaskNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineTask;
        node->next = NULL;
        return node;
}


AstNode *mkDefineMessageNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = DefineMessage;
        node->next = NULL;
        return node;
}


AstNode *mkIncludeRunoffFileNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = IncludeRunoffFile;
        node->next = NULL;
        return node;
}


AstNode *mkMessageIdentifierNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = MessageIdentifier;
        node->next = NULL;
        return node;
}


AstNode *mkStructMemberNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructMember;
        node->next = NULL;
        return node;
}


AstNode *mkParameterNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Parameter;
        node->next = NULL;
        return node;
}


AstNode *mkStructTypeNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructType;
        node->next = NULL;
        return node;
}


AstNode *mkArrayTypeNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ArrayType;
        node->next = NULL;
        return node;
}


AstNode *mkWhileNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = While;
        node->next = NULL;
        return node;
}


AstNode *mkForNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = For;
        node->next = NULL;
        return node;
}


AstNode *mkSwitchNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Switch;
        node->next = NULL;
        return node;
}


AstNode *mkSwitchCaseNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = SwitchCase;
        node->next = NULL;
        return node;
}


AstNode *mkIfNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = If;
        node->next = NULL;
        return node;
}


AstNode *mkElseIfNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ElseIf;
        node->next = NULL;
        return node;
}


AstNode *mkElseNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Else;
        node->next = NULL;
        return node;
}


AstNode *mkReceiveNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Receive;
        node->next = NULL;
        return node;
}


AstNode *mkReceiveCaseNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ReceiveCase;
        node->next = NULL;
        return node;
}


AstNode *mkVarDeclNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = VarDecl;
        node->next = NULL;
        return node;
}


AstNode *mkBinaryOperationNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = BinaryOperation;
        node->next = NULL;
        return node;
}


AstNode *mkVariableLocationNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = VariableLocation;
        node->next = NULL;
        return node;
}


AstNode *mkStructLocationNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = StructLocation;
        node->next = NULL;
        return node;
}


AstNode *mkArrayLocationNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = ArrayLocation;
        node->next = NULL;
        return node;
}


AstNode *mkUnaryOperationNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = UnaryOperation;
        node->next = NULL;
        return node;
}


AstNode *mkFunctionCallNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = FunctionCall;
        node->next = NULL;
        return node;
}


AstNode *mkAssignmentNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = Assignment;
        node->next = NULL;
        return node;
}


AstNode *mkTernaryOperatorNode(){
        AstNode *node = malloc(sizeof(AstNode));
        node->tag = TernaryOperator;
        node->next = NULL;
        return node;
}
