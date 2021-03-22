#include "symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static symbolTable* SymbolTable;
static symbolTable* current;

void insertSymbol(symbol* symbol) {
	symbol->next = current->symbols;
	current->symbols = symbol;
}

void initializeSymbolTables(void) {
	SymbolTable = (symbolTable*) malloc(sizeof(symbolTable));
	SymbolTable->previous = NULL;
	SymbolTable->symbols = NULL;
	current = SymbolTable;
}

void openScope(void) {
	symbolTable* new = (symbolTable*) malloc(sizeof(symbolTable));
	new->previous = current;
	new->symbols = NULL;
	current = new;
}

void closeScope(void) {
	current = current->previous;
}

symbol* retrieveSymbol(char* name) {
	symbolTable* currentTable = current;
	for (;currentTable != NULL; currentTable = currentTable->previous) {
		symbol* tempSymbol = currentTable->symbols;
		for (;tempSymbol != NULL; tempSymbol = tempSymbol->next) {
			if (strcmp(tempSymbol->name, name) == 0) {
				return tempSymbol;
			}
		}
	}
	return NULL;
}
