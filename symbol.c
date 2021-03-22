#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"

static SymbolTable* symbolTable;
static SymbolTable* current;
extern char *filename; /* defined in runoff.c */

Symbol *enterSymbol(char *, Type*);

int insertSymbol(AstNode *node, Type *type) {
	Symbol *symbol;
	char *name = node->node.Identifier.identifier;

	symbol = declaredLocally(name);
	if(symbol != NULL){
		printf("%s:%d: Name \"%s\" is already declared on line %d\n", filename, node->linenum, name, symbol->node->linenum);
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
	enterSymbol("turnOn", 0);
	enterSymbol("turnOff", 0);
	enterSymbol("readInput", 0);
	enterSymbol("readAnalogInput", 0);
	enterSymbol("delay", 0);
	enterSymbol("inputPin", 0);
	enterSymbol("outputPin", 0);
	enterSymbol("analogInputPin", 0);
	enterSymbol("analogOutputPin", 0);
	enterSymbol("setOutput", 0);
	enterSymbol("analogWrite", 0);
	enterSymbol("length", 0);
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