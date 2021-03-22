#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"

static SymbolTable* symbolTable;
static SymbolTable* current;

int insertSymbol(AstNode *node, int type) {
	Symbol *symbol;
	char *name = node->node.Identifier.identifier;

	if(declaredLocally(name)){
		printf("Name %s overloaded\n", name);
		return 1;
	}

	symbol = malloc(sizeof(Symbol));
	symbol->name = name;
	symbol->type = type;
	symbol->next = current->symbols;
	current->symbols = symbol;
	node->node.Identifier.symbol = symbol;
	return 0;
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
