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
	char *idstr = prettyprint(node.identifier);
	char *intlitstr = prettyprint(node.int_literal);
	char *result = smprintf("const %s %s;", idstr, intlitstr);
	free(idstr);
	free(intlitstr);
	return result;
}

static char *ppDefineFunction(DefineFunctionNode node){
	char *idstr = prettyprint(node.identifier);
	char *paramsstr = prettyprint(node.parameters);	/* Should print a comma seperated list of params */
	char *typestr = prettyprint(node.type);
	char *stmtsstr = prettyprint(node.statements);
	char *result = smprintf("function %s(%s) -> %s {%s}", idstr, paramsstr, typestr, stmtsstr);
	free(idstr);
	free(paramsstr);
	free(typestr);
	free(stmtsstr);
	return result;
}

static char *ppDefineTask(DefineTaskNode node){
	char *idstr = prettyprint(node.identifier);
	char *paramsstr = prettyprint(node.parameters); /* should print a comma seperated list of params */
	char *stmtsstr = prettyprint(node.statements);
	char *result = smprintf("task %s(%s) {%s}", idstr, paramsstr, stmtsstr);
	free(idstr);
	free(paramsstr);
	free(stmtsstr);
	return result;
}

static char *ppDefineMessage(DefineMessageNode node){
	char *result = smprintf("messages {");
	char *old;
	char *msgstr;
	AstNode *msg = node.messagesIdentifiers;

	while(msg != NULL){
		old = result;
		msgstr = prettyprint(msg);
		result = smprintf("%s\n\t%s;", old, msgstr);
		free(old);
		free(msgstr);
		msg = msg->next;
	}
	old = result;
	result = smprintf("%s\n}", old);
	free(old);
	return result;
}

static char *ppIncludeRunoffFile(IncludeRunoffFileNode node){
	char *idstr = prettyprint(node.identifier);
	char *result = smprintf("include(%s);", idstr);
	free(idstr);
	return result;
}

static char *ppMessageIdentifier(MessageIdentifierNode node){
	char *idstr = prettyprint(node.identifier);
	char *params;
	char *result;

	if(node.parameters == NULL)
		result = smprintf("%s", idstr);
	else{
		params = prettyprint(node.parameters); /* should print a comma seperated list of params */
		result = smprintf("%s(%s)", idstr, params);
		free(params);
	}
	return result;
}

static char *ppStructMember(StructMemberNode node){
	char *idstr = prettyprint(node.identifier);
	char *typestr = prettyprint(node.type);
	char *result = smprintf("%s %s", idstr, typestr);
	free(idstr);
	free(typestr);
	return result;
}

static char *ppParameter(ParameterNode node){
	char *typestr = prettyprint(node.type);
	char *idstr = prettyprint(node.identifier);
	char *result = smprintf("%s %s", typestr, idstr);
	free(typestr);
	free(idstr);
	return result;
}

static char *ppStructType(StructTypeNode node){
	char *idstr = prettyprint(node.identifier);
	char *result = smprintf("struct %s", idstr);
	free(idstr);
	return result;
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