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
void handleDefineFunction(AstNode *function);
void handleDefineTask(AstNode *function);
void handleMessageIdentifier(AstNode *function);
void checkIdentifierTypes(AstNode *tree);
static int errors;
extern char *filename; /* defined and set in runoff.c */

int buildSymbolTable(AstNode *tree){
	initializeSymbolTables();
	processNode(tree);

	checkIdentifierTypes(tree);

	if(errors)
		printf("BuildSymbolTable failed with %d errors\n", errors);

	return errors;
}

void checkIdentifierTypes(AstNode *tree){
	if(tree->tag == Identifier){
		Symbol *sym = tree->node.Identifier.symbol;
		if(sym == NULL){
			errors++;
			printf("Symbol for %s is NULL on line %d\n", tree->node.Identifier.identifier, tree->linenum);
		} else if(sym->type == NULL){
			errors++;
			printf("Symbol type for %s is NULL on line %d\n", tree->node.Identifier.identifier, tree->linenum);
		} else {
			/*
			printf("Symbol for %s is %d on line %d\n", tree->node.Identifier.identifier, sym->type->tag, tree->linenum);
			*/
		}
	} else {
		AstNode *children = getChildren(tree);
		for(;children!=NULL;children=children->next){
			checkIdentifierTypes(children);
		}
	}
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
	case DefineFunction:
		handleDefineFunction(node);

		return NULL;
	case DefineTask:
		handleDefineTask(node);
		return NULL;
	case DefineStruct:
		errors += insertSymbol(node->node.DefineStruct.identifier, 0);
		sym = retrieveSymbol(node->node.DefineStruct.identifier);
		openScope();
		sym->type = mkStructTypeDiscriptor(getCurrentSymbolTable());
		scopeopened = 1;
		break;
	case DefineMessage: break; /* Nothing */
	case IncludeRunoffFile: break; /* Nothing */
	case MessageIdentifier:
		handleMessageIdentifier(node);
		return NULL;
	case StructMember:
		vartype = processNode(node->node.StructMember.type);
		errors += insertSymbol(node->node.StructMember.identifier, vartype);
		break;
	case Parameter:
		vartype = processNode(node->node.Parameter.type);
		errors += insertSymbol(node->node.Parameter.identifier, vartype);
		break;
	case BuiltinType:
		return mkBuiltinTypeDiscriptor(node->node.BuiltinType.type);
	case StructType:
		sym = retrieveSymbol(node->node.StructType.identifier);
		if(sym == NULL)
			undeclaredError(node->node.StructType.identifier);
		else{
			updateSymbolId(node->node.StructType.identifier, sym);
			type = sym->type;
		}
		break;
	case ArrayType:
		vartype = processNode(node->node.ArrayType.type);
		if(node->node.ArrayType.int_literal == NULL){
			return mkArrayTypeDiscriptor(vartype, -1);
		} else {
			return mkArrayTypeDiscriptor(vartype, node->node.ArrayType.int_literal->node.IntLiteral.value);
		}
	case While:
	case For:  /* fallthrough */
	case Switch:  /* fallthrough */
	case SwitchCase: /* fallthrough */
	case Else: /* fallthrough */
	case Receive: /* fallthrough */
		openScope();
		scopeopened = 1;
		break;
	case ReceiveCase:
		sym = retrieveSymbol(node->node.ReceiveCase.messageName);
		if(sym == NULL)
			undeclaredError(node->node.ReceiveCase.messageName);
		else
			updateSymbolId(node->node.ReceiveCase.messageName, sym);
		openScope();

		processNodes(node->node.ReceiveCase.dataNames);
		processNodes(node->node.ReceiveCase.statements);

		closeScope();
		return NULL;
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
		errors += insertSymbol(node->node.VarDecl.identifier, vartype);
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
	case Identifier:
		sym = retrieveSymbol(node);
		if(sym == NULL)
			undeclaredError(node);
		else
			updateSymbolId(node, sym);
		break; /* Nothing */
	case PinLiteral: break; /* Nothing */
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
			updateSymbolId(identifier, sym);
			if(n->tag == StructLocation)
				table = sym->type->tags.typeStruct.fields;
			n = next;
		}
	}
}

void handleDefineFunction(AstNode *function){
	AstNode *Children;
	AstNode *tmp;
	Type **para_types;
	Type *t;
	Symbol *sym;
	int i = 0, parameter_length;

	errors += insertSymbol(function->node.DefineFunction.identifier, NULL);

	openScope();
	Children = concat_node(function->node.DefineFunction.parameters, function->node.DefineFunction.statements);
	processNodes(Children);

	parameter_length = nodeLength(function->node.DefineFunction.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.DefineFunction.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}

	t = mkFunctionTypeDiscriptor(parameter_length, para_types, processNode(function->node.DefineFunction.type));

	closeScope();

	sym = retrieveSymbol(function->node.DefineFunction.identifier);
	sym->type = t;
}

void handleDefineTask(AstNode *function){
	/* Copy pasted from handleDefineFunction. */
	AstNode *Children;
	AstNode *tmp;
	Type **para_types;
	Type *t;
	Symbol *sym;
	int i = 0, parameter_length;

	errors += insertSymbol(function->node.DefineTask.identifier, NULL);

	openScope();
	Children = concat_node(function->node.DefineTask.parameters, function->node.DefineTask.statements);
	processNodes(Children);

	parameter_length = nodeLength(function->node.DefineTask.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.DefineTask.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}

	t = mkTaskTypeDiscriptor(parameter_length, para_types);

	closeScope();

	sym = retrieveSymbol(function->node.DefineTask.identifier);
	sym->type = t;
}

void handleMessageIdentifier(AstNode *function){
	/* Copy pasted from handleDefineTask. */
	AstNode *tmp;
	Type **para_types;
	Type *t;
	Symbol *sym;
	int i = 0, parameter_length;

	errors += insertSymbol(function->node.MessageIdentifier.identifier, NULL);

	openScope();
	processNodes(function->node.MessageIdentifier.parameters);

	parameter_length = nodeLength(function->node.MessageIdentifier.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.MessageIdentifier.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}

	t = mkMessageTypeDiscriptor(parameter_length, para_types);

	closeScope();

	sym = retrieveSymbol(function->node.MessageIdentifier.identifier);
	sym->type = t;
}