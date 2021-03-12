#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ast.h"
#include "phases.h"

static char *smprintf(char *fmt, ...);

static char *ppProg(ProgNode node);
static char *ppDefineConst(DefineConstNode node);
static char *ppDefineFunction(DefineFunctionNode node);
static char *ppDefineTask(DefineTaskNode node);
static char *ppDefineMessage(DefineMessageNode node);
static char *ppIncludeRunoffFile(IncludeRunoffFileNode node);
static char *ppMessageIdentifier(MessageIdentifierNode node);
static char *ppStructMember(StructMemberNode node);
static char *ppParameter(ParameterNode node);
static char *ppStructType(StructTypeNode node);
static char *ppArrayType(ArrayTypeNode node);
static char *ppWhile(WhileNode node);
static char *ppFor(ForNode node);
static char *ppSwitch(SwitchNode node);
static char *ppSwitchCase(SwitchCaseNode node);
static char *ppIf(IfNode node);
static char *ppElseIf(ElseIfNode node);
static char *ppElse(ElseNode node);
static char *ppReceive(ReceiveNode node);
static char *ppReceiveCase(ReceiveCaseNode node);
static char *ppVarDecl(VarDeclNode node);
static char *ppBinaryOperation(BinaryOperationNode node);
static char *ppVariableLocation(VariableLocationNode node);
static char *ppStructLocation(StructLocationNode node);
static char *ppArrayLocation(ArrayLocationNode node);
static char *ppUnaryOperation(UnaryOperationNode node);
static char *ppFunctionCall(FunctionCallNode node);
static char *ppAssignment(AssignmentNode node);
static char *ppTernaryOperator(TernaryOperatorNode node);

char *prettyprint(AstNode *node)
{
	switch(node->tag){
	case Prog:
		return ppProg(node->node.Prog);
	case DefineConst:
		return ppDefineConst(node->node.DefineConst);
	case DefineFunction:
		return ppDefineFunction(node->node.DefineFunction);
	case DefineTask:
		return ppDefineTask(node->node.DefineTask);
	case DefineMessage:
		return ppDefineMessage(node->node.DefineMessage);
	case IncludeRunoffFile:
		return ppIncludeRunoffFile(node->node.IncludeRunoffFile);
	case MessageIdentifier:
		return ppMessageIdentifier(node->node.MessageIdentifier);
	case StructMember:
		return ppStructMember(node->node.StructMember);
	case Parameter:
		return ppParameter(node->node.Parameter);
	case StructType:
		return ppStructType(node->node.StructType);
	case ArrayType:
		return ppArrayType(node->node.ArrayType);
	case While:
		return ppWhile(node->node.While);
	case For:
		return ppFor(node->node.For);
	case Switch:
		return ppSwitch(node->node.Switch);
	case SwitchCase:
		return ppSwitchCase(node->node.SwitchCase);
	case If:
		return ppIf(node->node.If);
	case ElseIf:
		return ppElseIf(node->node.ElseIf);
	case Else:
		return ppElse(node->node.Else);
	case Receive:
		return ppReceive(node->node.Receive);
	case ReceiveCase:
		return ppReceiveCase(node->node.ReceiveCase);
	case VarDecl:
		return ppVarDecl(node->node.VarDecl);
	case BinaryOperation:
		return ppBinaryOperation(node->node.BinaryOperation);
	case VariableLocation:
		return ppVariableLocation(node->node.VariableLocation);
	case StructLocation:
		return ppStructLocation(node->node.StructLocation);
	case ArrayLocation:
		return ppArrayLocation(node->node.ArrayLocation);
	case UnaryOperation:
		return ppUnaryOperation(node->node.UnaryOperation);
	case FunctionCall:
		return ppFunctionCall(node->node.FunctionCall);
	case Assignment:
		return ppAssignment(node->node.Assignment);
	case TernaryOperator:
		return ppTernaryOperator(node->node.TernaryOperator);
	default:
		return "";
	}
}

static char *ppProg(ProgNode node){
	char *result = smprintf("");
	char *old;
	char *toplevelstr;
	AstNode *toplevels = node.toplevels;

	while(toplevels != NULL){
		old = result;
		toplevelstr = prettyprint(toplevels);
		result = smprintf("%s\n%s", old, toplevelstr);
		free(old);
		free(toplevelstr);
		toplevels = toplevels->next;
	}
	return result;
}

static char *ppDefineConst(DefineConstNode node){
return smprintf("const toplevel");
}

static char *ppDefineFunction(DefineFunctionNode node){
return smprintf("function toplevel");
}

static char *ppDefineTask(DefineTaskNode node){
return smprintf("task toplevel");
}

static char *ppDefineMessage(DefineMessageNode node){
return "";
}

static char *ppIncludeRunoffFile(IncludeRunoffFileNode node){
return "";
}

static char *ppMessageIdentifier(MessageIdentifierNode node){
return "";
}

static char *ppStructMember(StructMemberNode node){
return "";
}

static char *ppParameter(ParameterNode node){
return "";
}

static char *ppStructType(StructTypeNode node){
return "";
}

static char *ppArrayType(ArrayTypeNode node){
return "";
}

static char *ppWhile(WhileNode node){
return "";
}

static char *ppFor(ForNode node){
return "";
}

static char *ppSwitch(SwitchNode node){
return "";
}

static char *ppSwitchCase(SwitchCaseNode node){
return "";
}

static char *ppIf(IfNode node){
return "";
}

static char *ppElseIf(ElseIfNode node){
return "";
}

static char *ppElse(ElseNode node){
return "";
}

static char *ppReceive(ReceiveNode node){
return "";
}

static char *ppReceiveCase(ReceiveCaseNode node){
return "";
}

static char *ppVarDecl(VarDeclNode node){
return "";
}

static char *ppBinaryOperation(BinaryOperationNode node){
return "";
}

static char *ppVariableLocation(VariableLocationNode node){
return "";
}

static char *ppStructLocation(StructLocationNode node){
return "";
}

static char *ppArrayLocation(ArrayLocationNode node){
return "";
}

static char *ppUnaryOperation(UnaryOperationNode node){
return "";
}

static char *ppFunctionCall(FunctionCallNode node){
return "";
}

static char *ppAssignment(AssignmentNode node){
return "";
}

static char *ppTernaryOperator(TernaryOperatorNode node){
return smprintf("%s %s", "hey", "bob");
}

/* printf but it returns a string allocated by malloc */
static char *smprintf(char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	char *str = malloc(1000000);
	vsprintf(str, fmt, args);
	va_end(args);
	str = realloc(str, strlen(str) + 1);
	return str;
}