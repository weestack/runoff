#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "auxiliary.h"

static char *prettyprintlist(AstNode *node, char *sep, int end);

/* GLOBAL VARIABLES */
/* follows the order of the operators enum from ast.h */
char *operatorNames[] = {
	"&&",
	"||",
	">=",
	"<=",
	"==",
	"!=",
	"++",
	"--",
	"<",
	">",
	"%",
	"+",
	"-",
	"*",
	"/",
	"!",
	"&",
	"|",
	">>",
	"<<",
	"^",
	"~"
};

/* follows the order of the builintypes enum from ast.h */
char *builtintypeNames[] = {
	"uint8",
	"uint16",
	"uint32",
	"uint64",
	"int8",
	"int16",
	"int32",
	"int64",
	"unknownInt",
	"float",
	"void",
	"bool",
	"msg",
	"taskid",
	"pinid"
};

char *prettyprint(AstNode *node)
{
	/* temporary variables used by the prints.
	   they are all free'd at the end of this function. */
	char *id = NULL;
	char *params = NULL;
	char *type = NULL;
	char *stmts = NULL;
	char *fields = NULL;
	char *messages = NULL;
	char *intlit = NULL;
	char *exprA = NULL;
	char *exprB = NULL;
	char *exprC = NULL;
	char *cases = NULL;
	char *elsepart = NULL;
	char *message = NULL;
	char *tmp = NULL;
	char *op = NULL;
	char *location = NULL;
	char *indices = NULL;
	char *args = NULL;

	/* this is the return value of the prettyprint */
	char *result = NULL;

	if(node == NULL)
		return smprintf("");

	switch(node->tag){
	case Prog:
		result = prettyprintlist(node->node.Prog.toplevels, "\n\n", 0);
		break;
	case DefineFunction:
		id = prettyprint(node->node.DefineFunction.identifier);
		params = prettyprintlist(node->node.DefineFunction.parameters, ", ", 0);
		type = prettyprint(node->node.DefineFunction.type);
		stmts = prettyprintlist(node->node.DefineFunction.statements, "\n", 1);
		result = smprintf("function %s(%s) -> %s {\n%s}", id, params, type, stmts);
		break;
	case DefineTask:
		id = prettyprint(node->node.DefineTask.identifier);
		params = prettyprintlist(node->node.DefineTask.parameters, ", ", 0);
		stmts = prettyprintlist(node->node.DefineTask.statements, "\n", 1);
		result = smprintf("task %s(%s) {\n%s}", id, params, stmts);
		break;
	case DefineStruct:
		id = prettyprint(node->node.DefineStruct.identifier);
		fields = prettyprintlist(node->node.DefineStruct.fields, ";\n", 1);
		result = smprintf("struct %s {\n%s}", id, fields);
		break;
	case DefineMessage:
		messages = prettyprintlist(node->node.DefineMessage.messagesIdentifiers, ";\n", 1);
		result = smprintf("messages {\n%s}", messages);
		break;
	case MessageIdentifier:
		id = prettyprint(node->node.MessageIdentifier.identifier);
		if(node->node.MessageIdentifier.parameters == NULL)
			result = smprintf("%s", id);
		else{
			params = prettyprintlist(node->node.MessageIdentifier.parameters, ", ", 0);
			result = smprintf("%s{%s}", id, params);
		}
		break;
	case StructMember:
		id = prettyprint(node->node.StructMember.identifier);
		type = prettyprint(node->node.StructMember.type);
		result = smprintf("%s %s", type, id);
		break;
	case Parameter:
		type = prettyprint(node->node.Parameter.type);
		id = prettyprint(node->node.Parameter.identifier);
		result = smprintf("%s %s", type, id);
		break;
	case BuiltinType:
		result = smprintf("%s", builtintypeNames[node->node.BuiltinType.type]);
		break;
	case StructType:
		id = prettyprint(node->node.StructType.identifier);
		result = smprintf("struct %s", id);
		break;
	case ArrayType:
		type = prettyprint(node->node.ArrayType.elementType);
		indices = prettyprintlist(node->node.ArrayType.dimensions, "][", 0);
		result = smprintf("%s[%s]", type, indices);
		break;
	case While:
		exprA = prettyprint(node->node.While.expression);
		stmts = prettyprintlist(node->node.While.statements, "\n", 1);
		result = smprintf("while(%s) {\n%s}", exprA, stmts);
		break;
	case For:
		exprA = prettyprint(node->node.For.expressionInit);
		exprB = prettyprint(node->node.For.expressionTest);
		exprC = prettyprint(node->node.For.expressionUpdate);
		stmts = prettyprintlist(node->node.For.statements, "\n", 1);
		result = smprintf("for(%s; %s; %s) {\n%s}", exprA, exprB, exprC, stmts);
		break;
	case Switch:
		exprA = prettyprint(node->node.Switch.expression);
		cases = prettyprintlist(node->node.Switch.cases, "\n", 1);
		result = smprintf("switch(%s) {\n%s}", exprA, cases);
		break;
	case SwitchCase:
		intlit = prettyprint(node->node.SwitchCase.literal);
		stmts = prettyprintlist(node->node.SwitchCase.statements, "\n", 1);
		if(node->node.SwitchCase.literal == NULL)
			result = smprintf("default: %s", stmts);
		else
			result = smprintf("case %s: %s", intlit, stmts);
		break;
	case If:
		exprA = prettyprint(node->node.If.expression);
		stmts = prettyprintlist(node->node.If.statements, "\n", 1);
		if(node->node.If.elsePart != NULL)
			elsepart = prettyprint(node->node.If.elsePart);
		else
			elsepart = smprintf("");
		result = smprintf("if(%s) {\n%s} %s", exprA, stmts, elsepart);
		break;
	case ElseIf:
		exprA = prettyprint(node->node.ElseIf.expression);
		stmts = prettyprintlist(node->node.ElseIf.statements, "\n", 1);
		elsepart = prettyprint(node->node.ElseIf.elsePart);
		result = smprintf("elseif(%s) {\n%s} %s", exprA, stmts, elsepart);
		break;
	case Else:
		stmts = prettyprintlist(node->node.Else.statements, "\n", 1);
		result = smprintf("else {\n%s}", stmts);
		break;
	case Receive:
		cases = prettyprintlist(node->node.Receive.cases, "\n", 1);
		result = smprintf("receive {\n%s}", cases);
		break;
	case ReceiveCase:
		message = prettyprint(node->node.ReceiveCase.messageName);
		args = prettyprintlist(node->node.ReceiveCase.dataNames, ", ", 0);
		stmts = prettyprintlist(node->node.ReceiveCase.statements, "\n", 1);
		if(node->node.ReceiveCase.messageName == NULL)
			result = smprintf("default: %s", stmts);
		else
			result = smprintf("case %s{%s}: %s", message, args, stmts);
		break;
	case VarDecl:
		type = prettyprint(node->node.VarDecl.type);
		id = prettyprint(node->node.VarDecl.identifier);
		if(node->node.VarDecl.expression != NULL){
			exprA = prettyprint(node->node.VarDecl.expression);
			tmp = smprintf("%s %s = %s", type, id, exprA);
		}else
			tmp = smprintf("%s %s", type, id);
		if(node->node.VarDecl.toplevel)
			result = smprintf("%s;", tmp);
		else
			result = smprintf("%s", tmp);
		break;
	case BinaryOperation:
		exprA = prettyprint(node->node.BinaryOperation.expression_left);
		op = smprintf("%s", operatorNames[node->node.BinaryOperation.operator]);
		exprB = prettyprint(node->node.BinaryOperation.expression_right);
		result = smprintf("(%s %s %s)", exprA, op, exprB);
		break;
	case VariableLocation:
		id = prettyprint(node->node.VariableLocation.identifier);
		result = smprintf("%s", id);
		break;
	case StructLocation:
		id = prettyprint(node->node.StructLocation.identifier);
		location = prettyprint(node->node.StructLocation.location);
		result = smprintf("%s.%s", id, location);
		break;
	case ArrayLocation:
		id = prettyprint(node->node.ArrayLocation.identifier);
		indices = prettyprintlist(node->node.ArrayLocation.indicies, "][", 0);
		result = smprintf("%s[%s]", id, indices);
		break;
	case UnaryOperation:
		op = smprintf("%s", operatorNames[node->node.UnaryOperation.operator]);
		exprA = prettyprint(node->node.UnaryOperation.expression);
		if(node->node.UnaryOperation.fix == Prefix)
			result = smprintf("(%s%s)", op, exprA);
		else
			result = smprintf("(%s%s)", exprA, op);
		break;
	case FunctionCall:
		id = prettyprint(node->node.FunctionCall.identifier);
		args = prettyprintlist(node->node.FunctionCall.arguments, ", ", 0);
		result = smprintf("%s(%s)", id, args);
		break;
	case Assignment:
		location = prettyprint(node->node.Assignment.location);
		exprA = prettyprint(node->node.Assignment.expression);
		result = smprintf("(%s = %s)", location, exprA);
		break;
	case TernaryOperator:
		exprA = prettyprint(node->node.TernaryOperator.expressionTest);
		exprB = prettyprint(node->node.TernaryOperator.expressionTrue);
		exprC = prettyprint(node->node.TernaryOperator.expressionFalse);
		result = smprintf("%s ? %s : %s", exprA, exprB, exprC);
		break;
	case Identifier:
		result = smprintf(node->node.Identifier.identifier);
		break;
	case PinLiteral:
		result = smprintf("%ldP", node->node.PinLiteral.value);
		break;
	case IntLiteral:
		result = smprintf("%ld", node->node.IntLiteral.value);
		break;
	case FloatLiteral:
		result = smprintf("%f", node->node.FloatLiteral.value);
		break;
	case BoolLiteral:
		result = smprintf("%s", node->node.BoolLiteral.value ? "true" : "false");
		break;
	case MessageLiteral:
		id = prettyprint(node->node.MessageLiteral.identifier);
		args = prettyprintlist(node->node.MessageLiteral.arguments, ", ", 0);
		result = smprintf("%s{%s}", id, args);
		break;
	case Return:
		if(node->node.Return.expression != NULL){
			exprA = prettyprint(node->node.Return.expression);
			result = smprintf("return %s;", exprA);
		}else
			result = smprintf("return;");
		break;
	case Spawn:
		id = prettyprint(node->node.Spawn.identifier);
		args = prettyprintlist(node->node.Spawn.arguments, ", ", 0);
		result = smprintf("spawn %s(%s)", id, args);
		break;
	case Send:
		exprA = prettyprint(node->node.Send.message);
		exprB = prettyprint(node->node.Send.receiver);
		result = smprintf("send %s to %s;", exprA, exprB);
		break;
	case ExprStmt:
		exprA = prettyprint(node->node.ExprStmt.expression);
		result = smprintf("%s;", exprA);
		break;
	default:
		result = smprintf("");
	}
	
	free(id);
	free(params);
	free(type);
	free(stmts);
	free(fields);
	free(messages);
	free(intlit);
	free(exprA);
	free(exprB);
	free(exprC);
	free(cases);
	free(elsepart);
	free(message);
	free(tmp);
	free(op);
	free(location);
	free(indices);
	free(args);
	return result;
}

static char *prettyprintlist(AstNode *node, char *sep, int end){
	char *result;
	char *prev;
	char *childstr;
	AstNode *child;

	if(node == NULL)
		return smprintf("");

	child = node->next;
	result = prettyprint(node);

	while(child != NULL){
		prev = result;
		childstr = prettyprint(child);
		result = smprintf("%s%s%s", prev, sep, childstr);
		free(prev);
		free(childstr);
		child = child->next;
	}
	if(end){
		prev = result;
		result = smprintf("%s%s", prev, sep);
		free(prev);
	}
	return result;
}
