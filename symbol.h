typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct Type Type;
typedef struct ArrayTypeDiscriptor ArrayTypeDiscriptor;
typedef struct FunctionTypeDiscriptor FunctionTypeDiscriptor;
typedef struct BuiltinTypeDiscriptor BuiltinTypeDiscriptor;
typedef struct StructTypeDiscriptor StructTypeDiscriptor;

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

struct ArrayTypeDiscriptor {
	Type *elementType;
	int size;
};

struct FunctionTypeDiscriptor {
	int arity;
	Type **parameterTypes;
	Type *returnType;
};

struct BuiltinTypeDiscriptor {
	int builtinType;
};

struct StructTypeDiscriptor {
	SymbolTable *fields;
};


struct Type {
	int tag;
	union {
		ArrayTypeDiscriptor typeArray;
		FunctionTypeDiscriptor typeFunction;
		BuiltinTypeDiscriptor typeBuiltin;
		StructTypeDiscriptor typeStruct;
	} tags;
};

void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
Symbol *declaredLocally(char*);
SymbolTable *getCurrentSymbolTable(void);