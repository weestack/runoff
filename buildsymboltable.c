#include <stdio.h>
#include "symbol.h"
#include "ast.h"
#include "phases.h"

void processNode(AstNode*);
void processNodes(AstNode *);
void undeclaredError(AstNode *);
void updateSymbolId(AstNode *, Symbol *);
AstNode *getChildren(AstNode *);

static int errors;
extern char *filename; /* defined and set in runoff.c */

void buildSymbolTable(AstNode *tree){
	initializeSymbolTables();
	processNode(tree);
	if(errors)
		printf("BuildSymbolTable failed with %d errors\n", errors);
}

void processNode(AstNode *node){
	int scopeopened = 0;
	Symbol *sym;
	AstNode *child;

	if(node == NULL)
		return;

	switch(node->tag){
	case Prog: break; /* Nothing */
	case DefineConst:
		errors += insertSymbol(node->node.DefineConst.identifier, 0); /* TYPE FIX */
		break;
	case DefineFunction:
		errors += insertSymbol(node->node.DefineFunction.identifier, 0); /* TYPE FIX */
		openScope();
		scopeopened = 1;
		break;
	case DefineTask:
		errors += insertSymbol(node->node.DefineTask.identifier, 0); /* TYPE FIX */
		openScope();
		scopeopened = 1;
		break;
	case DefineStruct:
		errors += insertSymbol(node->node.DefineStruct.identifier, 0); /* TYPE FIX */
		openScope();
		scopeopened = 1;
		break;
	case DefinePinid:
		errors += insertSymbol(node->node.DefinePinid.identifier, 0); /* TYPE FIX */
		break;
	case DefineMessage: break; /* Nothing */
	case IncludeRunoffFile: break; /* Nothing */
	case MessageIdentifier:
		errors += insertSymbol(node->node.MessageIdentifier.identifier, 0); /* TYPE FIX */
		break;
	case StructMember:
		errors += insertSymbol(node->node.StructMember.identifier, 0); /* TYPE FIX */
		break;
	case Parameter:
		errors += insertSymbol(node->node.Parameter.identifier, 0); /* TYPE FIX */
		break;
	case BuiltinType: break; /* Nothing */
	case StructType:
		sym = retrieveSymbol(node->node.StructType.identifier);
		if(sym == NULL)
			undeclaredError(node->node.StructType.identifier);
		else
			updateSymbolId(node->node.StructType.identifier, sym);
		break;
	case ArrayType: break; /* Nothing */
	case While:
	case For:  /* fallthrough */
	case Switch:  /* fallthrough */
	case SwitchCase: /* fallthrough */
	case Else: /* fallthrough */
	case Receive: /* fallthrough */
	case ReceiveCase: /* fallthrough */
		openScope();
		scopeopened = 1;
		break;
	case If:
		openScope();
		child = append_node(node->node.If.expression, node->node.If.statements);
		processNodes(child);
		closeScope();
		processNode(node->node.If.elsePart);
		return; /* special case */
	case ElseIf:
	 	openScope();
		child = append_node(node->node.ElseIf.expression, node->node.ElseIf.statements);
		processNodes(child);
		closeScope();
		processNode(node->node.ElseIf.elsePart);
		return; /* special case */
	case VarDecl:
		/* what about int i = i + 2???? */
		errors += insertSymbol(node->node.VarDecl.identifier, 0); /* TYPE FIX */
		break;
	case BinaryOperation: break; /* Nothing */
	case VariableLocation:
		sym = retrieveSymbol(node->node.VariableLocation.identifier);
		if(sym == NULL)
			undeclaredError(node->node.VariableLocation.identifier);
		else
			updateSymbolId(node->node.VariableLocation.identifier, sym);
		break;
	case StructLocation: break; /* Nothing yet fix later :) */
	case ArrayLocation:
		sym = retrieveSymbol(node->node.ArrayLocation.identifier);
		if(sym == NULL)
			undeclaredError(node->node.ArrayLocation.identifier);
		else
			updateSymbolId(node->node.ArrayLocation.identifier, sym);
		break;
	case UnaryOperation: break; /* Nothing */
	case FunctionCall:
		sym = retrieveSymbol(node->node.FunctionCall.identifier);
		if(sym == NULL)
			undeclaredError(node->node.FunctionCall.identifier);
		else
			updateSymbolId(node->node.FunctionCall.identifier, sym);
		break;
	case Assignment: break; /* Nothing */
	case TernaryOperator: break; /* Nothing */
	case Identifier: break; /* Nothing */
	case IntLiteral: break; /* Nothing */
	case FloatLiteral: break; /* Nothing */
	case BoolLiteral: break; /* Nothing */
	case Return: break; /* Nothing */
	case Spawn:
		sym = retrieveSymbol(node->node.Spawn.identifier);
		if(sym == NULL)
			undeclaredError(node->node.Spawn.identifier);
		else
			updateSymbolId(node->node.Spawn.identifier, sym);
		break;
	case Send: break; /* Nothing */
	case ExprStmt: break; /* Nothing */
	}

	child = getChildren(node);
	processNodes(child);

	if(scopeopened)
		closeScope();
}

void processNodes(AstNode *nodes){
	AstNode *child;
	for(child = nodes; child != NULL; child = child->next)
		processNode(child);
}

void undeclaredError(AstNode *node){
	char *name = node->node.Identifier.identifier;
	errors++;
	printf("%s:%d: Undeclared symbol \"%s\"\n", filename, node->linenum, name);
}

void updateSymbolId(AstNode *node, Symbol *s){
	node->node.Identifier.symbol = s;
}

AstNode *getChildren(AstNode *node){
	AstNode *result = NULL;

	switch(node->tag){
	case Prog:
		result = node->node.Prog.toplevels;
		break;
	case DefineConst:
		result = append_node(node->node.DefineConst.identifier, node->node.DefineConst.int_literal);
		break;
	case DefineFunction:
		result = append_node(node->node.DefineFunction.identifier, node->node.DefineFunction.parameters);
		result = append_node(result, node->node.DefineFunction.type);
		result = append_node(result, node->node.DefineFunction.statements);
		break;
	case DefineTask:
		result = append_node(node->node.DefineTask.identifier, node->node.DefineTask.parameters);
		result = append_node(result, node->node.DefineTask.statements);
		break;
	case DefineStruct:
		result = append_node(node->node.DefineStruct.identifier, node->node.DefineStruct.fields);
		break;
	case DefinePinid:
		result = append_node(node->node.DefinePinid.identifier, node->node.DefinePinid.int_literal);
		break;
	case DefineMessage:
		result = node->node.DefineMessage.messagesIdentifiers;
		break;
	case IncludeRunoffFile:
		result = node->node.IncludeRunoffFile.identifier;
		break;
	case MessageIdentifier:
		result = append_node(node->node.MessageIdentifier.identifier, node->node.MessageIdentifier.parameters);
		break;
	case StructMember:
		result = append_node(node->node.StructMember.identifier, node->node.StructMember.type);
		break;
	case Parameter:
		result = append_node(node->node.Parameter.type, node->node.Parameter.identifier);
		break;
	case BuiltinType:
		break;
	case StructType:
		result = node->node.StructType.identifier;
		break;
	case ArrayType:
		result = append_node(node->node.ArrayType.type, node->node.ArrayType.int_literal);
		break;
	case While:
		result = append_node(node->node.While.expression, node->node.While.statements);
		break;
	case For:
		result = append_node(node->node.For.expressionInit, node->node.For.expressionTest);
		result = append_node(result, node->node.For.expressionUpdate);
		result = append_node(result, node->node.For.statements);
		break;
	case Switch:
		result = append_node(node->node.Switch.expression, node->node.Switch.cases);
		break;
	case SwitchCase:
		result = append_node(node->node.SwitchCase.literal, node->node.SwitchCase.statements);
		break;
	case If:
		result = append_node(node->node.If.expression, node->node.If.statements);
		result = append_node(result, node->node.If.elsePart);
		break;
	case ElseIf:
		result = append_node(node->node.ElseIf.expression, node->node.ElseIf.statements);
		result = append_node(result, node->node.ElseIf.elsePart);
		break;
	case Else:
		result = node->node.Else.statements;
		break;
	case Receive:
		result = node->node.Receive.cases;
		break;
	case ReceiveCase:
		result = append_node(node->node.ReceiveCase.messageName, node->node.ReceiveCase.dataNames);
		result = append_node(result, node->node.ReceiveCase.statements);
		break;
	case VarDecl:
		result = append_node(node->node.VarDecl.type, node->node.VarDecl.identifier);
		result = append_node(result, node->node.VarDecl.expression);
		break;
	case BinaryOperation:
		result = append_node(node->node.BinaryOperation.expression_left, node->node.BinaryOperation.expression_right);
		break;
	case VariableLocation:
		result = node->node.VariableLocation.identifier;
		break;
	case StructLocation:
		result = append_node(node->node.StructLocation.identifier, node->node.StructLocation.location);
		break;
	case ArrayLocation:
		result = append_node(node->node.ArrayLocation.identifier, node->node.ArrayLocation.indicies);
		break;
	case UnaryOperation:
		result = node->node.UnaryOperation.expression;
		break;
	case FunctionCall:
		result = append_node(node->node.FunctionCall.identifier, node->node.FunctionCall.arguments);
		break;
	case Assignment:
		result = append_node(node->node.Assignment.location, node->node.Assignment.expression);
		break;
	case TernaryOperator:
		result = append_node(node->node.TernaryOperator.expressionTest, node->node.TernaryOperator.expressionTrue);
		result = append_node(result, node->node.TernaryOperator.expressionFalse);
		break;
	case Identifier:
		break;
	case IntLiteral:
		break;
	case FloatLiteral:
		break;
	case BoolLiteral:
		break;
	case Return:
		result = node->node.Return.expression;
		break;
	case Spawn:
		result = append_node(node->node.Spawn.identifier, node->node.Spawn.arguments);
		break;
	case Send:
		result = append_node(node->node.Send.message, node->node.Send.receiver);
		break;
	case ExprStmt:
		result = node->node.ExprStmt.expression;
		break;
	}

	return result;
}