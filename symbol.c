#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "auxiliary.h"

static SymbolTable* symbolTable;
static SymbolTable* current;
static Symbol *enterSymbol(char *, Type*);
static Symbol *declaredLocally(char *);
static void insertBuiltinFunctions(void);
static InitializeInfo *mkInitInfo(Type *);

int insertSymbol(AstNode *node, Type *type) {
	Symbol *symbol;
	char *name = node->node.Identifier.identifier;

	symbol = declaredLocally(name);
	if(symbol != NULL){
		eprintf(node->linenum, "Name \"%s\" is already declared on line %d\n", name, symbol->linenum);
		return 1;
	}

	symbol = enterSymbol(name, type);
	symbol->linenum = node->linenum;
	symbol->first = node;
	node->node.Identifier.symbol = symbol;
	return 0;
}

void initializeSymbolTables(void) {
	symbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
	symbolTable->previous = NULL;
	symbolTable->symbols = NULL;
	current = symbolTable;

	insertBuiltinFunctions();
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

static Symbol *declaredLocally(char *name){
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
	symbol->globalvar = 0;
	symbol->initInfo = mkInitInfo(type);
	symbol->used = 0;
	symbol->next = current->symbols;
	current->symbols = symbol;

	return symbol;
}

static InitializeInfo **mkInitInfoArray(Type *type, AstNode *dims){
	int size = dims->node.IntLiteral.value;
	InitializeInfo **info = malloc(sizeof(InitializeInfo*) * size);
	int i;
	for(i = 0; i < size; i++){
		if(dims->next == NULL)
			info[i] = mkInitInfo(type);
		else{
			info[i] = malloc(sizeof(InitializeInfo));
			info[i]->arrayInitialized = mkInitInfoArray(type, dims->next);
		}
	}
	return info;
}

static InitializeInfo *mkInitInfo(Type *t) {
	InitializeInfo *info;

	if(t == NULL || !(t->tag == BuiltinTypeTag || t->tag == ArrayTypeTag || t->tag == StructTypeTag))
		return NULL;

	info = malloc(sizeof(InitializeInfo));
	info->varInitialized = 0;
	if(t->tag == ArrayTypeTag)
		info->arrayInitialized = mkInitInfoArray(t->tags.typeArray.elementType, t->tags.typeArray.dimensions);
	else
		info->arrayInitialized = NULL;

	return info;
	/* Some case for structs also */
}

SymbolTable *getCurrentSymbolTable(void){
	return current;
}

Type* mkBuiltinTypeDescriptor(int type){
	Type *t = malloc(sizeof(Type));
	t->tag = BuiltinTypeTag;
	t->tags.typeBuiltin.builtinType = type;

	return t;
}

Type* mkArrayTypeDescriptor(Type *elementType, AstNode *dimensions){
	Type *t = malloc(sizeof(Type));
	t->tag = ArrayTypeTag;
	t->tags.typeArray.elementType = elementType;
	t->tags.typeArray.dimensions = dimensions;

	return t;
}

Type* mkFunctionTypeDescriptor(int arity, Type **parameterTypes, Type *returnType){
	Type *t = malloc(sizeof(Type));
	t->tag = FunctionTypeTag;
	t->tags.typeFunction.arity = arity;
	t->tags.typeFunction.parameterTypes = parameterTypes;
	t->tags.typeFunction.returnType = returnType;

	return t;
}

Type* mkStructTypeDescriptor(char *name, SymbolTable *fields){
	Type *t = malloc(sizeof(Type));
	t->tag = StructTypeTag;
	t->tags.typeStruct.name = name;
	t->tags.typeStruct.fields = fields;
	return t;
}

Type* mkTaskTypeDescriptor(int arity, Type **parameterTypes){
	Type *t = malloc(sizeof(Type));
	t->tag = TaskTypeTag;
	t->tags.typeTask.arity = arity;
	t->tags.typeTask.parameterTypes = parameterTypes;

	return t;
}

Type* mkMessageTypeDescriptor(int arity, Type **parameterTypes){
	Type *t = malloc(sizeof(Type));
	t->tag = MessageTypeTag;
	t->tags.typeMessage.arity = arity;
	t->tags.typeMessage.parameterTypes = parameterTypes;

	return t;
}

void insertBuiltinFunctions(void) {
	/* insert symbols for builin funcions here :) 8))) */
	Type **turnOnParameters = malloc(sizeof(Type*) * 1);
	Type **turnOffParameters = malloc(sizeof(Type*) * 1);
	Type **readInputParameters = malloc(sizeof(Type*) * 1);
	Type **readAnalogInputParameters = malloc(sizeof(Type*) * 1);
	Type **delayParameters = malloc(sizeof(Type*) * 1);
	Type **advancedInputPinParameters = malloc(sizeof(Type*) * 4);
	Type **inputPinParameters = malloc(sizeof(Type*) * 1);
	Type **outputPinParameters = malloc(sizeof(Type*) * 1);
	Type **setOutputParameters = malloc(sizeof(Type*) * 2);
	Type **analogWriteParameters = malloc(sizeof(Type*) * 2);

	turnOnParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	turnOffParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	readInputParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	readAnalogInputParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	delayParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypeUint32);
	advancedInputPinParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	advancedInputPinParameters[1] = mkBuiltinTypeDescriptor(BuiltinTypeTaskid);
	advancedInputPinParameters[2] = mkBuiltinTypeDescriptor(BuiltinTypeMsg);
	advancedInputPinParameters[3] = mkBuiltinTypeDescriptor(BuiltinTypeMsg);
	inputPinParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	outputPinParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	setOutputParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	setOutputParameters[1] = mkBuiltinTypeDescriptor(BuiltinTypeBool);
	analogWriteParameters[0] = mkBuiltinTypeDescriptor(BuiltinTypePinid);
	analogWriteParameters[1] = mkBuiltinTypeDescriptor(BuiltinTypeUint8);

	enterSymbol("turnOn", mkFunctionTypeDescriptor(1,turnOnParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("turnOff", mkFunctionTypeDescriptor(1,turnOffParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("readInput", mkFunctionTypeDescriptor(1,readInputParameters,mkBuiltinTypeDescriptor(BuiltinTypeBool)));
	enterSymbol("readAnalogInput", mkFunctionTypeDescriptor(1,readAnalogInputParameters,mkBuiltinTypeDescriptor(BuiltinTypeUint16)));
	enterSymbol("delay", mkFunctionTypeDescriptor(1,delayParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("advancedInputPin", mkFunctionTypeDescriptor(4,advancedInputPinParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("inputPin", mkFunctionTypeDescriptor(1,inputPinParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("outputPin", mkFunctionTypeDescriptor(1,outputPinParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("setOutput", mkFunctionTypeDescriptor(2,setOutputParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("analogWrite", mkFunctionTypeDescriptor(2,analogWriteParameters,mkBuiltinTypeDescriptor(BuiltinTypeVoid)));
	enterSymbol("self", mkFunctionTypeDescriptor(0, NULL, mkBuiltinTypeDescriptor(BuiltinTypeTaskid)));
}
