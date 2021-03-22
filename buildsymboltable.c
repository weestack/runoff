#include <stdio.h>
#include "symbol.h"
#include "ast.h"
#include "phases.h"

void processNode(AstNode*);
void processNodes(AstNode *);
void undeclaredError(AstNode *);
void updateSymbolId(AstNode *, Symbol *);

static int errors;
extern char *filename; /* defined and set in runoff.c */

int buildSymbolTable(AstNode *tree){
	initializeSymbolTables();
	processNode(tree);
	if(errors)
		printf("BuildSymbolTable failed with %d errors\n", errors);
	return errors;
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
		processNode(node->node.VarDecl.expression);
		errors += insertSymbol(node->node.VarDecl.identifier, 0); /* TYPE FIX */
		return; /* special case */
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
