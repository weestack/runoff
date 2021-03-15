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
static char *ppDefineStruct(DefineStructNode node);
static char *ppDefineMessage(DefineMessageNode node);
static char *ppIncludeRunoffFile(IncludeRunoffFileNode node);
static char *ppMessageIdentifier(MessageIdentifierNode node);
static char *ppStructMember(StructMemberNode node);
static char *ppParameter(ParameterNode node);
static char *ppBuiltinType(BuiltinTypeNode node);
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
static char *ppIdentifier(IdentifierNode node);
static char *ppIntLiteral(IntLiteralNode node);
static char *ppFloatLiteral(FloatLiteralNode node);
static char *ppBoolLiteral(BoolLiteralNode node);
static char *ppReturn(ReturnNode node);

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
	case DefineStruct:
		return ppDefineStruct(node->node.DefineStruct);
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
	case BuiltinType:
		return ppBuiltinType(node->node.BuiltinType);
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
	case Identifier:
		return ppIdentifier(node->node.Identifier);
	case IntLiteral:
		return ppIntLiteral(node->node.IntLiteral);
	case FloatLiteral:
		return ppFloatLiteral(node->node.FloatLiteral);
	case BoolLiteral:
		return ppBoolLiteral(node->node.BoolLiteral);
	case Return:
		return ppReturn(node->node.Return);
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

static char *ppDefineStruct(DefineStructNode node){
	char *idstr = prettyprint(node.identifier);
	char *fieldsstr = prettyprint(node.fields); /* should print a ; seperated list of fields */
	char *result = smprintf("struct %s { %s };", idstr, fieldsstr);
	free(idstr);
	free(fieldsstr);
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

static char *ppBuiltinType(BuiltinTypeNode node){
	char *typestr = smprintf("tytytyt%d", node.type); /* should look the name up in a table */
	return typestr;
}

static char *ppStructType(StructTypeNode node){
	char *idstr = prettyprint(node.identifier);
	char *result = smprintf("struct %s", idstr);
	free(idstr);
	return result;
}

static char *ppArrayType(ArrayTypeNode node){
	char *typeStr = prettyprint(node.type);
	char *int_literalStr = prettyprint(node.int_literal);
	char *result = smprintf("%s[%s]", typeStr, int_literalStr);
	free(typeStr); free(int_literalStr);
	return result;
}

static char *ppWhile(WhileNode node){
	char *expressionStr = prettyprint(node.expression);
	char *statementsStr = prettyprint(node.statements);
	char *result = smprintf("while(%s) {%s}", expressionStr, statementsStr);
	free(expressionStr); free(statementsStr);
	return result;
}

static char *ppFor(ForNode node){
	char *expressionInitStr = prettyprint(node.expressionInit);
	char *expressionTestStr = prettyprint(node.expressionTest);
	char *expressionUpdateStr = prettyprint(node.expressionUpdate);
	char *statementsStr = prettyprint(node.statements);
	char *result = smprintf("for(%s; %s; %s) {%s}", expressionInitStr, expressionTestStr, expressionUpdateStr, statementsStr);
	free(expressionInitStr); free(expressionTestStr);
	free(expressionUpdateStr); free(statementsStr);
	return result;
}

static char *ppSwitch(SwitchNode node){
	char *expressionStr = prettyprint(node.expression);
	char *casesStr = prettyprint(node.cases);
	char *result = smprintf("switch(%s) {%s}", expressionStr, casesStr);
	free(expressionStr); free(casesStr);
	return result;
}

static char *ppSwitchCase(SwitchCaseNode node){
	char *literalStr = prettyprint(node.literal);
	char *statementsStr = prettyprint(node.statements);
	char *result = smprintf("case %s: %s", literalStr, statementsStr);
	free(literalStr); free(statementsStr);
	return result;
}

static char *ppIf(IfNode node){
	char *expressionStr = prettyprint(node.expression);
	char *statementsStr = prettyprint(node.statements);
	char *elsePartStr;
	char *result;

	if(node.elsePart)
		elsePartStr = prettyprint(node.elsePart);
	else
		elsePartStr = smprintf("");
	result = smprintf("if(%s) {%s} %s", expressionStr, statementsStr, elsePartStr);
	free(expressionStr); free(statementsStr);  free(elsePartStr);
	return result;
}

static char *ppElseIf(ElseIfNode node){
	char *expressionStr = prettyprint(node.expression);
	char *statementsStr = prettyprint(node.statements);
	char *elsePartStr = prettyprint(node.elsePart);
	char *result = smprintf("elseif(%s) {%s} %s", expressionStr, statementsStr, elsePartStr);
	free(expressionStr); free(statementsStr);  free(elsePartStr);
	return result;
}

static char *ppElse(ElseNode node){
	char *statementsStr = prettyprint(node.statements);
	char *result = smprintf("else {%s}", statementsStr);
	free(statementsStr);
	return result;
}

static char *ppReceive(ReceiveNode node){
	char *casesStr = prettyprint(node.cases);
	char *result = smprintf("receive {%s}", casesStr);
	free(casesStr);
	return result;
}

static char *ppReceiveCase(ReceiveCaseNode node){
	char *messageNameStr = prettyprint(node.messageName);
	char *dataNamesStr = prettyprint(node.dataNames);
	char *statementsStr = prettyprint(node.statements);
	char *result = smprintf("case %s(%s):%s", messageNameStr, dataNamesStr, statementsStr);
	free(messageNameStr); free(dataNamesStr); free(statementsStr);
	return result;
}

static char *ppVarDecl(VarDeclNode node){
	char *typestr = prettyprint(node.type);
	char *identifierStr = prettyprint(node.identifier);
	char *expressionStr;
	char *result;

	if(node.expression)
		expressionStr = prettyprint(node.expression);
	else
		expressionStr = smprintf("default value for type %s");
	result = smprintf("%s %s = %s", typestr, identifierStr, expressionStr);
	free(typestr);
	free(identifierStr);
	free(expressionStr);
	return result;
}

static char *ppBinaryOperation(BinaryOperationNode node){
	char *expressionLeftStr = prettyprint(node.expression_left);
	/* char *operatorStr = prettyprint(); fix, make print function for ops */
	char *expressionRightStr = prettyprint(node.expression_right);
	char *result = smprintf("%s %s", expressionLeftStr, expressionRightStr);
	free(expressionLeftStr); free(expressionRightStr);
	return result;
}

static char *ppVariableLocation(VariableLocationNode node){
	char *identifierStr = prettyprint(node.identifier);
	char *result = smprintf("%s %s", identifierStr);
	free(identifierStr);
	return result;
}

static char *ppStructLocation(StructLocationNode node){
	char *identifier = prettyprint(node.identifier);
	char *location = prettyprint(node.location);
	char *result = smprintf("%s.%s", identifier, location);
	free(identifier); free(location);
	return result;
}

static char *ppArrayLocation(ArrayLocationNode node){
	char *identifier = prettyprint(node.identifier);
	char *indices = prettyprint(node.indicies);
	char *result = smprintf("%s[%s]", identifier, indices);
	free(identifier); free(indices);
	return result;
}

static char *ppUnaryOperation(UnaryOperationNode node){
	char *operatorStr = smprintf("OP"); /* fix me */
	char *expressionStr = prettyprint(node.expression);
	char *result;

	if(node.fix == prefix)
		result = smprintf("%s %s", operatorStr, expressionStr);
	else
		result = smprintf("%s %s", expressionStr, operatorStr);
	free(operatorStr);
	free(expressionStr);
	return result;
}

static char *ppFunctionCall(FunctionCallNode node){
	char *identifierStr = prettyprint(node.identifier);
	char *argumentsStr = prettyprint(node.arguments);
	char *result = smprintf("%s(%s)", identifierStr, argumentsStr);
	free(identifierStr); free(argumentsStr);
	return result;
}

static char *ppAssignment(AssignmentNode node){
	char *locationStr = prettyprint(node.location);
	char *expressionStr = prettyprint(node.expression);
	char *result = smprintf("%s = %s", locationStr, expressionStr);
	free(locationStr); free(expressionStr);
	return result;
}

static char *ppTernaryOperator(TernaryOperatorNode node){
	char *expressionTestStr = prettyprint(node.expressionTest);
	char *expressionTrueStr = prettyprint(node.expressionTrue);
	char *expressionFalseStr = prettyprint(node.expressionFalse);
	char *result = smprintf("(%s) ? %s : %s", expressionTestStr,
	 expressionTrueStr, expressionFalseStr);

	free(expressionTestStr);free(expressionTrueStr);free(expressionFalseStr);
	return result;
}

static char *ppIdentifier(IdentifierNode node){
	return smprintf(node.identifier);
}

static char *ppIntLiteral(IntLiteralNode node){
	return smprintf("%d", node.value);
}

static char *ppFloatLiteral(FloatLiteralNode node){
	return smprintf("%f", node.value);
}

static char *ppBoolLiteral(BoolLiteralNode node){
	return smprintf("%s", node.value ? "true" : "false");
}

static char *ppReturn(ReturnNode node){
	char *exprstr;
	char *result;

	if(node.expression)
		exprstr = prettyprint(node.expression);
	else
		exprstr = smprintf("");

	result = smprintf("return %s;", exprstr);
	free(exprstr);
	return result;
}

/* printf but it returns a string allocated by malloc */
static char *smprintf(char *fmt, ...){
	va_list args;
	char *str = malloc(1000000);
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);
	str = realloc(str, strlen(str) + 1);
	return str;
}
