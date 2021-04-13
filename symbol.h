typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct Type Type;
typedef struct ArrayTypeDescriptor ArrayTypeDescriptor;
typedef struct FunctionTypeDescriptor FunctionTypeDescriptor;
typedef struct BuiltinTypeDescriptor BuiltinTypeDescriptor;
typedef struct StructTypeDescriptor StructTypeDescriptor;
typedef struct TaskTypeDescriptor TaskTypeDescriptor;
typedef struct MessageTypeDescriptor MessageTypeDescriptor;
Type* mkBuiltinTypeDescriptor(int);
Type* mkArrayTypeDescriptor(Type *, int);
Type* mkFunctionTypeDescriptor(int, Type **, Type *);
Type* mkTaskTypeDescriptor(int, Type **);
Type* mkStructTypeDescriptor(char *name, SymbolTable *);
Type* mkMessageTypeDescriptor(int, Type**);
Type* mkAnyTypeDescriptor(void);

struct AstNode; /* declared fully in ast.h */

struct Symbol {
	char* name;
	Type *type;
	int globalvar;
	int initialized;

	struct AstNode *node; /* the node which inserted the symbol */
	Symbol* next;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};

/* if size equal to -1 then it is array[]. */
struct ArrayTypeDescriptor {
	Type *elementType;
	int size;
};

struct FunctionTypeDescriptor {
	int arity;
	Type **parameterTypes;
	Type *returnType;
};

struct TaskTypeDescriptor {
	int arity;
	Type **parameterTypes;
};

struct BuiltinTypeDescriptor {
	int builtinType;
};

struct StructTypeDescriptor {
	char *name;
	SymbolTable *fields;
};

struct MessageTypeDescriptor {
	int arity;
	Type **parameterTypes;
};


struct Type {
	int tag;
	union {
		ArrayTypeDescriptor typeArray;
		FunctionTypeDescriptor typeFunction;
		TaskTypeDescriptor typeTask;
		BuiltinTypeDescriptor typeBuiltin;
		StructTypeDescriptor typeStruct;
		MessageTypeDescriptor typeMessage;
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