#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"
#include "phases.h"

Type *processNode(AstNode*);
void processNodes(AstNode *);
void undeclaredError(AstNode *);
void updateSymbolId(AstNode *, Symbol *);
void handleStructLocation(AstNode *);
static int errors;
extern char *filename; /* defined and set in runoff.c */

int buildSymbolTable(AstNode *tree){
	initializeSymbolTables();
	processNode(tree);
	if(errors)
		printf("BuildSymbolTable failed with %d errors\n", errors);
	return errors;
}

/* process node CAN return a type if it makes sense */
Type *processNode(AstNode *node){
	int scopeopened = 0;
	Symbol *sym;
	AstNode *child;
	Type *type = NULL; /* the return value of this function */
	Type *vartype;

	if(node == NULL)
		return NULL;

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
		sym = retrieveSymbol(node->node.DefineStruct.identifier);
		openScope();
		sym->type = malloc(sizeof(Type));
		sym->type->structfields = getCurrentSymbolTable();
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
		vartype = processNode(node->node.StructMember.type);
		errors += insertSymbol(node->node.StructMember.identifier, vartype); /* TYPE FIX */
		break;
	case Parameter:
		vartype = processNode(node->node.Parameter.type);
		errors += insertSymbol(node->node.Parameter.identifier, vartype); /* TYPE FIX */
		break;
	case BuiltinType: break; /* Nothing */
	case StructType:
		sym = retrieveSymbol(node->node.StructType.identifier);
		if(sym == NULL)
			undeclaredError(node->node.StructType.identifier);
		else{
			updateSymbolId(node->node.StructType.identifier, sym);
			type = sym->type;
		}
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
		return NULL; /* special case */
	case ElseIf:
	 	openScope();
		child = append_node(node->node.ElseIf.expression, node->node.ElseIf.statements);
		processNodes(child);
		closeScope();
		processNode(node->node.ElseIf.elsePart);
		return NULL; /* special case */
	case VarDecl:
		vartype = processNode(node->node.VarDecl.type);
		processNode(node->node.VarDecl.expression);
		errors += insertSymbol(node->node.VarDecl.identifier, vartype); /* TYPE FIX */
		return NULL; /* special case */
	case BinaryOperation: break; /* Nothing */
	case VariableLocation:
		sym = retrieveSymbol(node->node.VariableLocation.identifier);
		if(sym == NULL)
			undeclaredError(node->node.VariableLocation.identifier);
		else
			updateSymbolId(node->node.VariableLocation.identifier, sym);
		break;
	case StructLocation:
		/* This is special, so move it to a function */
		handleStructLocation(node);
		return NULL;
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

	return type;
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

void handleStructLocation(AstNode *node){
	AstNode *n = node;
	AstNode *next = n;
	SymbolTable *table = NULL; /* default is the current table */
	while(next != NULL){
		AstNode *identifier;
		Symbol *sym;

		switch(n->tag){
		case StructLocation:
			identifier = n->node.StructLocation.identifier;
			next = n->node.StructLocation.location;
			break;
		case VariableLocation:
			identifier = n->node.VariableLocation.identifier;
			next = NULL;
			break;
		case ArrayLocation:
			identifier = n->node.ArrayLocation.identifier;
			next = NULL;
			processNodes(n->node.ArrayLocation.indicies);
			break;
		}

		if(table == NULL)
			sym = retrieveSymbol(identifier);
		else
			sym = retrieveSymbolFromTable(identifier, table);

		if(sym == NULL){
			undeclaredError(identifier);
			return;
		}else{
			if(n->tag == StructLocation)
				table = sym->type->structfields;
			n = next;
		}
	}
}