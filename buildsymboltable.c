#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "auxiliary.h"

Type *processNode(AstNode*);
void processNodes(AstNode *, int);
void undeclaredError(AstNode *);
void updateSymbolId(AstNode *, Symbol *);
void handleStructLocation(AstNode *);
void handleDefineFunction(AstNode *function);
void handleDefineTask(AstNode *function);
void handleMessageIdentifier(AstNode *function);
void handleReceiveCase(AstNode *node);
void handleAssignment(AstNode *node);
void checkIdentifierTypes(AstNode *tree);
static int errors;
static Symbol *currentfunc; /* the symbol of the current function */

int buildSymbolTable(AstNode *tree){
	initializeSymbolTables();
	processNode(tree);

	/* this next is mostly usefull for debugging and is thus disabled
	checkIdentifierTypes(tree);
	*/

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
		AstNode *children = tree->children;
		for(;children!=NULL;children=children->chain){
			checkIdentifierTypes(children);
		}
	}
}

/* process node CAN return a type if it makes sense */
Type *processNode(AstNode *node){
	int scopeopened = 0;
	Symbol *sym;
	Type *type = NULL; /* the return value of this function */
	Type *vartype;

	if(node == NULL)
		return NULL;

	switch(node->tag){
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
		sym->type = mkStructTypeDescriptor(sym->name, getCurrentSymbolTable());
		scopeopened = 1;
		break;
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
		sym = retrieveSymbol(node->node.Parameter.identifier);
		setInitialized(sym->initInfo, sym->type);
		break;
	case BuiltinType:
		return mkBuiltinTypeDescriptor(node->node.BuiltinType.type);
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
		vartype = processNode(node->node.ArrayType.elementType);
		return mkArrayTypeDescriptor(vartype, node->node.ArrayType.dimensions);
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
		if(node->node.ReceiveCase.messageName != NULL)
			handleReceiveCase(node);
		else{
			openScope();
			processNodes(node->node.ReceiveCase.statements, 0);
			closeScope();
		}
		return NULL;
	case If:
		openScope();
		processNodes(node->node.If.expression, 0);
		processNodes(node->node.If.statements, 0);
		closeScope();
		processNode(node->node.If.elsePart);
		return NULL; /* special case */
	case ElseIf:
	 	openScope();
		processNodes(node->node.ElseIf.expression, 0);
		processNodes(node->node.ElseIf.statements, 0);
		closeScope();
		processNode(node->node.ElseIf.elsePart);
		return NULL; /* special case */
	case VarDecl:
		vartype = processNode(node->node.VarDecl.type);
		processNode(node->node.VarDecl.expression);
		errors += insertSymbol(node->node.VarDecl.identifier, vartype);
		sym = retrieveSymbol(node->node.VarDecl.identifier);
		sym->globalvar = node->node.VarDecl.toplevel;
		if(node->node.VarDecl.expression != NULL)
			setInitialized(sym->initInfo, sym->type);
		return NULL; /* special case */
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
	case FunctionCall:
		sym = retrieveSymbol(node->node.FunctionCall.identifier);
		if(sym == NULL)
			undeclaredError(node->node.FunctionCall.identifier);
		else
			updateSymbolId(node->node.FunctionCall.identifier, sym);
		break;
	case Identifier:
		sym = retrieveSymbol(node);
		if(sym == NULL)
			undeclaredError(node);
		else
			updateSymbolId(node, sym);
		break;
	case MessageLiteral: 
		sym = retrieveSymbol(node->node.MessageLiteral.identifier);
		if(sym == NULL)
			undeclaredError(node->node.MessageLiteral.identifier);
		else
			updateSymbolId(node->node.MessageLiteral.identifier, sym);
		break;
	case Return:
		node->node.Return.functionsym = currentfunc;
		break;
	case Spawn:
		sym = retrieveSymbol(node->node.Spawn.identifier);
		if(sym == NULL)
			undeclaredError(node->node.Spawn.identifier);
		else
			updateSymbolId(node->node.Spawn.identifier, sym);
		break;
	case Assignment:
		handleAssignment(node);
		return NULL;
	}

	processNodes(node->children, 1);

	if(scopeopened)
		closeScope();

	return type;
}

void processNodes(AstNode *nodes, int usechain){
	AstNode *child = nodes;
	while(child != NULL){
		processNode(child);
		if(usechain)
			child = child->chain;
		else
			child = child->next;
	}
}

void undeclaredError(AstNode *node){
	char *name = node->node.Identifier.identifier;
	errors++;
	eprintf(node->linenum, "Undeclared symbol \"%s\"\n", name);
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
			processNodes(n->node.ArrayLocation.indicies, 0);
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
	AstNode *tmp;
	Type **para_types;
	Symbol *sym;
	int i = 0, parameter_length;

	errors += insertSymbol(function->node.DefineFunction.identifier, NULL);
	sym = retrieveSymbol(function->node.DefineFunction.identifier);
	currentfunc = sym;

	openScope();
	processNodes(function->node.DefineFunction.parameters, 0);
	processNodes(function->node.DefineFunction.statements, 0);

	parameter_length = nodeLength(function->node.DefineFunction.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.DefineFunction.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}
	
	sym->type = mkFunctionTypeDescriptor(parameter_length, para_types, processNode(function->node.DefineFunction.type));;

	closeScope();
	currentfunc = NULL;
}

void handleDefineTask(AstNode *function){
	/* Copy pasted from handleDefineFunction. */
	AstNode *tmp;
	Type **para_types;
	Type *t;
	Symbol *sym;
	int i = 0, parameter_length;

	errors += insertSymbol(function->node.DefineTask.identifier, NULL);

	openScope();
	processNodes(function->node.DefineTask.parameters, 0);
	processNodes(function->node.DefineTask.statements, 0);

	parameter_length = nodeLength(function->node.DefineTask.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.DefineTask.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}

	t = mkTaskTypeDescriptor(parameter_length, para_types);

	closeScope();

	sym = retrieveSymbol(function->node.DefineTask.identifier);
	sym->type = t;
	sym->first = function;
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
	processNodes(function->node.MessageIdentifier.parameters, 0);

	parameter_length = nodeLength(function->node.MessageIdentifier.parameters);

	para_types = malloc(sizeof(Type*)*parameter_length);
	for(tmp = function->node.MessageIdentifier.parameters; tmp != NULL; tmp=tmp->next){
		AstNode *identifier = tmp->node.Parameter.identifier;
		para_types[i] = identifier->node.Identifier.symbol->type;
		i++;
	}

	t = mkMessageTypeDescriptor(parameter_length, para_types);

	closeScope();

	sym = retrieveSymbol(function->node.MessageIdentifier.identifier);
	sym->type = t;
	sym->first = function;
}

void handleReceiveCase(AstNode *node){
	Symbol *sym = retrieveSymbol(node->node.ReceiveCase.messageName);
	AstNode *arg;
	Type **params = NULL;
	int paramnr = 0;
	int paramcount = 0;

	if(sym == NULL){
		undeclaredError(node->node.ReceiveCase.messageName);
		return;
	}else
		updateSymbolId(node->node.ReceiveCase.messageName, sym);

	if(sym->type->tag == MessageTypeTag){
		params = sym->type->tags.typeMessage.parameterTypes;
		paramcount = sym->type->tags.typeMessage.arity;
	}

	openScope();
	for(arg = node->node.ReceiveCase.dataNames;
		arg != NULL && paramnr < paramcount;
		arg = arg->next, paramnr++)
	{
		Symbol *tmpsym;
		errors += insertSymbol(arg, params[paramnr]);
		tmpsym = retrieveSymbol(arg);
		setInitialized(tmpsym->initInfo, tmpsym->type);
	}
	if(paramnr != paramcount || arg != NULL){
		errors++;
		eprintf(node->linenum,"Arity of message in receive case does not match message prototype for %s (should be %d but is %d)\n", sym->name, paramcount, nodeLength(node->node.ReceiveCase.dataNames));
	}
	processNodes(node->node.ReceiveCase.statements, 0);
	closeScope();
}

void handleAssignment(AstNode *node){
	AstNode *loc = node->node.Assignment.location;
	AstNode *expr = node->node.Assignment.expression;
	AstNode *id = NULL;
	Symbol *sym;
	Type *type;
	Type *elementType;
	AstNode *dims;
	InitializeInfo *initInfo;

	processNode(expr);
	processNode(loc);

	switch(loc->tag){
	case VariableLocation:
		id = loc->node.VariableLocation.identifier;
		sym = id->node.Identifier.symbol;
		setInitialized(sym->initInfo, sym->type);
		break;
	case ArrayLocation:
		id = loc->node.ArrayLocation.identifier;
		sym = id->node.Identifier.symbol;
		initInfo = sym->initInfo;
		type = sym->type;
		elementType = type->tags.typeArray.elementType;
		dims = type->tags.typeArray.dimensions;
		loc = loc->node.ArrayLocation.indicies;
		while(loc != NULL && loc->tag == IntLiteral){
			initInfo = initInfo->arrayInitialized[loc->node.IntLiteral.value];
			type = mkArrayTypeDescriptor(elementType, dims->next);
			loc = loc->next;
			dims = dims->next;
		}
		if(loc == NULL){
			if(dims == NULL)
				setInitialized(initInfo, elementType);
			else
				setInitialized(initInfo, type);
		}
		break;
	case StructLocation:
		id = loc->node.StructLocation.identifier;
		initInfo = id->node.Identifier.symbol->initInfo;
		loc = loc->node.StructLocation.location;

		while(loc->tag == StructLocation){
			StructInitializeInfo *sinfo;
			id = loc->node.StructLocation.identifier;
			for(sinfo = initInfo->structInitialized; sinfo != NULL; sinfo = sinfo->next){
				char *fieldname = sinfo->symbol->name;
				char *idname = id->node.Identifier.symbol->name;
				if(strcmp(fieldname, idname) == 0){
					initInfo = sinfo->info;
					break;
				}
			}
			loc = loc->node.StructLocation.location;
		}

		if(loc->tag == VariableLocation)
			id = loc->node.VariableLocation.identifier;
		else if(loc->tag == ArrayLocation)
			id = loc->node.ArrayLocation.identifier;
		
		setInitializedStructField(initInfo, id->node.Identifier.symbol->name);
		break;
	}
}
