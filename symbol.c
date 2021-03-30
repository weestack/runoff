#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"
#include "auxiliary.h"

static SymbolTable* symbolTable;
static SymbolTable* current;

Symbol *enterSymbol(char *, Type*);

int insertSymbol(AstNode *node, Type *type) {
	Symbol *symbol;
	char *name = node->node.Identifier.identifier;

	symbol = declaredLocally(name);
	if(symbol != NULL){
		eprintf(node->linenum, "Name \"%s\" is already declared on line %d\n", name, symbol->node->linenum);
		return 1;
	}

	symbol = enterSymbol(name, type);
	symbol->node = node;
	node->node.Identifier.symbol = symbol;
	return 0;
}

void initializeSymbolTables(void) {
	symbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
	symbolTable->previous = NULL;
	symbolTable->symbols = NULL;
	current = symbolTable;

	/* insert symbols for builin funcions here :) */
	enterSymbol("turnOn", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("turnOff", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("readInput", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_bool)));
	enterSymbol("readAnalogInput", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_int32)));
	enterSymbol("delay", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("inputPin", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("outputPin", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogInputPin", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogOutputPin", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("setOutput", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogWrite", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("length", mkFunctionTypeDiscriptor(0,NULL,mkBuiltinTypeDiscriptor(builtintype_int32)));
}

void openScope(void) {
	SymbolTable* new = (SymbolTable*) malloc(sizeof(SymbolTable));
	new->previous = current;
	new->symbols = NULL;
	current = new;
}

void closeScope(void) {
	current = current->previous;
}

Symbol* retrieveSymbol(AstNode *node) {
	SymbolTable* currentTable = current;
	Symbol *sym;
	for (;currentTable != NULL; currentTable = currentTable->previous){
		sym = retrieveSymbolFromTable(node, currentTable);
		if(sym != NULL)
			return sym;
	}

	return NULL;
}

Symbol *retrieveSymbolFromTable(AstNode *node, SymbolTable *table){
	char *name = node->node.Identifier.identifier;
	Symbol* tempSymbol = table->symbols;
	for (;tempSymbol != NULL; tempSymbol = tempSymbol->next) {
		if (strcmp(tempSymbol->name, name) == 0) {
			return tempSymbol;
		}
	}
	return NULL;
}

Symbol *declaredLocally(char *name){
	Symbol *tmp;
	for(tmp = current->symbols; tmp != NULL; tmp = tmp->next){
		if(strcmp(tmp->name, name) == 0)
			return tmp;
	}
	return NULL;
}

Symbol *enterSymbol(char *name, Type *type){
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->name = name;
	symbol->type = type;
	symbol->next = current->symbols;
	current->symbols = symbol;
	return symbol;
}

SymbolTable *getCurrentSymbolTable(void){
	return current;
}

Type* mkBuiltinTypeDiscriptor(int type){
	Type *t = malloc(sizeof(Type));
	t->tag = BuiltinTypeTag;
	t->tags.typeBuiltin.builtinType = type;

	return t;
}

Type* mkArrayTypeDiscriptor(Type *elementType, int size){
	Type *t = malloc(sizeof(Type));
	t->tag = ArrayTypeTag;
	t->tags.typeArray.elementType = elementType;
	t->tags.typeArray.size = size;

	return t;
}

Type* mkFunctionTypeDiscriptor(int arity, Type **parameterTypes, Type *returnType){
	Type *t = malloc(sizeof(Type));
	t->tag = FunctionTypeTag;
	t->tags.typeFunction.arity = arity;
	t->tags.typeFunction.parameterTypes = parameterTypes;
	t->tags.typeFunction.returnType = returnType;

	return t;
}

Type* mkStructTypeDiscriptor(char *name, SymbolTable *fields){
	Type *t = malloc(sizeof(Type));
	t->tag = StructTypeTag;
	t->tags.typeStruct.name = name;
	t->tags.typeStruct.fields = fields;
	return t;
}

Type* mkTaskTypeDiscriptor(int arity, Type **parameterTypes){
	Type *t = malloc(sizeof(Type));
	t->tag = TaskTypeTag;
	t->tags.typeTask.arity = arity;
	t->tags.typeTask.parameterTypes = parameterTypes;

	return t;
}

Type* mkMessageTypeDiscriptor(int arity, Type **parameterTypes){
	Type *t = malloc(sizeof(Type));
	t->tag = MessageTypeTag;
	t->tags.typeMessage.arity = arity;
	t->tags.typeMessage.parameterTypes = parameterTypes;

	return t;
}