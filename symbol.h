typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;

struct AstNode; /* declared fully in ast.h */

struct Symbol {
	char* name;
	int type;

	struct AstNode *node; /* the node which inserted the symbol */
	Symbol* next;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};

void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
Symbol *declaredLocally(char*);
