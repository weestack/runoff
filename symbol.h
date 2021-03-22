typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;

struct Symbol {
	char* 	name;
	char* 	type;
	int 	var;

	Symbol* next;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};

void initializeSymbolTables(void);
void insertSymbol(Symbol*);
void openScope(void);
void closeScope(void);
Symbol* retrieveSymbol(char*);
int declaredLocally(char*);
