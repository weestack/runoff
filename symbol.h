typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;

struct Symbol {
	char* name;
	int type;

	Symbol* next;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};

void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
int declaredLocally(char*);
