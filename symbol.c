#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"

static SymbolTable* symbolTable;
static SymbolTable* current;
extern char *filename; /* defined in runoff.c */

Symbol *enterSymbol(char *, int);

int insertSymbol(AstNode *node, int type) {
	Symbol *symbol;
	char *name = node->node.Identifier.identifier;

	if(declaredLocally(name)){
		printf("%s:%d: Name \"%s\" is already declared\n", filename, node->linenum, name);
		return 1;
	}

	symbol = enterSymbol(name, type);
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
	enterSymbol("setOutput", 0);
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
	char *name = node->node.Identifier.identifier;
	SymbolTable* currentTable = current;
	for (;currentTable != NULL; currentTable = currentTable->previous) {
		Symbol* tempSymbol = currentTable->symbols;
		for (;tempSymbol != NULL; tempSymbol = tempSymbol->next) {
			if (strcmp(tempSymbol->name, name) == 0) {
				return tempSymbol;
			}
		}
	}
	return NULL;
}

int declaredLocally(char *name){
	Symbol *tmp;
	for(tmp = current->symbols; tmp != NULL; tmp = tmp->next){
		if(strcmp(tmp->name, name) == 0)
			return 1;
	}
	return 0;
}

Symbol *enterSymbol(char *name, int type){
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->name = name;
	symbol->type = type;
	symbol->next = current->symbols;
	current->symbols = symbol;
	return symbol;
}