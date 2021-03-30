#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"
#include "auxiliary.h"

static SymbolTable* symbolTable;
static SymbolTable* current;

Symbol *enterSymbol(char *, Type*);
void insertBuiltinFunctions(void);

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

void insertBuiltinFunctions(void) {
	/* insert symbols for builin funcions here :) 8))) */
	Type **turnOnParameters = malloc(sizeof(Type*) * 1);
	Type **turnOffParameters = malloc(sizeof(Type*) * 1);
	Type **readInputParameters = malloc(sizeof(Type*) * 1);
	Type **readAnalogInputParameters = malloc(sizeof(Type*) * 1);
	Type **delayParameters = malloc(sizeof(Type*) * 1);
	Type **inputPinParameters = malloc(sizeof(Type*) * 4);
	Type **simpleInputPinParameters = malloc(sizeof(Type*) * 1);
	Type **outputPinParameters = malloc(sizeof(Type*) * 1);
	Type **analogInputPinParameters = malloc(sizeof(Type*) * 1);
	Type **analogOutputPinParameters = malloc(sizeof(Type*) * 1);
	Type **setOutputParameters = malloc(sizeof(Type*) * 2);
	Type **analogWriteParameters = malloc(sizeof(Type*) * 2);
	Type **lengthParameters = malloc(sizeof(Type*) * 1);

	turnOnParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	turnOffParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	readInputParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	readAnalogInputParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	delayParameters[0] = mkBuiltinTypeDiscriptor(builtintype_uint32);
	inputPinParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	inputPinParameters[1] = mkBuiltinTypeDiscriptor(builtintype_taskid);
	inputPinParameters[2] = mkBuiltinTypeDiscriptor(builtintype_msg);
	inputPinParameters[3] = mkBuiltinTypeDiscriptor(builtintype_msg);
	simpleInputPinParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	outputPinParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	analogInputPinParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	analogOutputPinParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	setOutputParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	setOutputParameters[1] = mkBuiltinTypeDiscriptor(builtintype_bool);
	analogWriteParameters[0] = mkBuiltinTypeDiscriptor(builtintype_pinid);
	analogWriteParameters[1] = mkBuiltinTypeDiscriptor(builtintype_uint16);
	lengthParameters[0] = mkArrayTypeDiscriptor(NULL, -1); /* Funny mans */

	enterSymbol("turnOn", mkFunctionTypeDiscriptor(1,turnOnParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("turnOff", mkFunctionTypeDiscriptor(1,turnOffParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("readInput", mkFunctionTypeDiscriptor(1,readInputParameters,mkBuiltinTypeDiscriptor(builtintype_bool)));
	enterSymbol("readAnalogInput", mkFunctionTypeDiscriptor(1,readAnalogInputParameters,mkBuiltinTypeDiscriptor(builtintype_uint16)));
	enterSymbol("delay", mkFunctionTypeDiscriptor(1,delayParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("inputPin", mkFunctionTypeDiscriptor(4,inputPinParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("simpleInputPin", mkFunctionTypeDiscriptor(1,simpleInputPinParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("outputPin", mkFunctionTypeDiscriptor(1,outputPinParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogInputPin", mkFunctionTypeDiscriptor(1,analogInputPinParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogOutputPin", mkFunctionTypeDiscriptor(1,analogOutputPinParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("setOutput", mkFunctionTypeDiscriptor(2,setOutputParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("analogWrite", mkFunctionTypeDiscriptor(2,analogWriteParameters,mkBuiltinTypeDiscriptor(builtintype_void)));
	enterSymbol("length", mkFunctionTypeDiscriptor(1,lengthParameters,mkBuiltinTypeDiscriptor(builtintype_unknownInt))); /* funny */
}