typedef struct symbol symbol;
typedef struct symbolTable symbolTable;

struct symbol {
	char* 	name;
	char* 	type;
	int 	var;

	symbol* next;
};

struct symbolTable {
	symbolTable*	previous;
	symbol* 		symbols;
};

void newGlobalTables();
void insertSymbol(symbol*);
void openScope();
void closeScope();
symbol* retrieveSymbol(char*);