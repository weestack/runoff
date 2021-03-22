typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct Type Type;

struct AstNode; /* declared fully in ast.h */

struct Symbol {
	char* name;
	Type *type;

	struct AstNode *node; /* the node which inserted the symbol */
	Symbol* next;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};

/* this is currently only used by the struct types to handle lines such as
   int i = hej.med.dig where .med must be a field in whatever struct type
   hej is an instance of, and .dig must be a field in that... */
struct Type {
	SymbolTable *structfields;
};

void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
Symbol *declaredLocally(char*);
SymbolTable *getCurrentSymbolTable(void);