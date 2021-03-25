typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct Type Type;
typedef struct ArrayTypeDiscriptor ArrayTypeDiscriptor;
typedef struct FunctionTypeDiscriptor FunctionTypeDiscriptor;
typedef struct BuiltinTypeDiscriptor BuiltinTypeDiscriptor;
typedef struct StructTypeDiscriptor StructTypeDiscriptor;
typedef struct TaskTypeDiscriptor TaskTypeDiscriptor;
typedef struct MessageTypeDiscriptor MessageTypeDiscriptor;
Type* mkBuiltinTypeDiscriptor(int);
Type* mkArrayTypeDiscriptor(Type *, int);
Type* mkFunctionTypeDiscriptor(int, Type **, Type *);
Type* mkTaskTypeDiscriptor(int, Type **);
Type* mkStructTypeDiscriptor(SymbolTable *);
Type* mkMessageTypeDiscriptor(int, Type**);

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

struct TaskTypeDiscriptor {
	int arity;
	Type **parameterTypes;
};

struct BuiltinTypeDiscriptor {
	int builtinType;
};

struct StructTypeDiscriptor {
	SymbolTable *fields;
};

struct MessageTypeDiscriptor {
	int arity;
	Type **parameterTypes;
};


struct Type {
	int tag;
	union {
		ArrayTypeDiscriptor typeArray;
		FunctionTypeDiscriptor typeFunction;
		TaskTypeDiscriptor typeTask;
		BuiltinTypeDiscriptor typeBuiltin;
		StructTypeDiscriptor typeStruct;
		MessageTypeDiscriptor typeMessage;
	} tags;
};

enum {
	ArrayTypeTag,
	FunctionTypeTag,
	TaskTypeTag,
	BuiltinTypeTag,
	StructTypeTag,
	MessageTypeTag
};

void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
Symbol *declaredLocally(char*);
SymbolTable *getCurrentSymbolTable(void);