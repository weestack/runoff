#include "symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static SymbolTable* symbolTable;
static SymbolTable* current;

void insertSymbol(Symbol* symbol) {
	symbol->next = current->symbols;
	current->symbols = symbol;
}

void initializeSymbolTables(void) {
	symbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
	symbolTable->previous = NULL;
	symbolTable->symbols = NULL;
	current = symbolTable;
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

Symbol* retrieveSymbol(char* name) {
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