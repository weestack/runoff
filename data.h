/* 
 * Enums comes first
 */

/* This enum has an element for each node type in the AST.
   It is used as a tag. */
enum NodeTypes {
	Prog,
	DefineFunction,
	DefineTask,
	DefineStruct,
	DefineMessage,
	MessageIdentifier,
	StructMember,
	Parameter,
	BuiltinType,
	StructType,
	ArrayType,
	While,
	For,
	Switch,
	SwitchCase,
	If,
	ElseIf,
	Else,
	Receive,
	ReceiveCase,
	VarDecl,
	BinaryOperation,
	VariableLocation,
	StructLocation,
	ArrayLocation,
	UnaryOperation,
	FunctionCall,
	Assignment,
	TernaryOperator,
	Identifier,
	PinLiteral,
	IntLiteral,
	FloatLiteral,
	BoolLiteral,
	MessageLiteral,
	Return,
	Spawn,
	Send,
	ExprStmt
};

/* This enum has an element for each of the binary and unary operators
   in the language. */
enum Operators {
	OpLogAnd,
	OpLogOr,
	OpGreaterEqual,
	OpSmallerEqual,
	OpEqual,
	OpNotEqual,
	OpIncrement,
	OpDecrement,
	OpSmallerThan,
	OpGreaterThan,
	OpMod,
	OpPlus,
	OpMinus,
	OpTimes,
	OpDivide,
	OpLogNot,
	OpBitAnd,
	OpBitOr,
	OpShiftRight,
	OpShiftLeft,
	OpBitXor,
	OpBitNot
};

/* Enum used to tell if a unary operator is to be printed in pre- or postfix form */
enum Fixity {
	Prefix,
	Postfix
};

/* This enum has one element for each type in the language */
enum BuiltinTypes {
	BuiltinTypeUint8,
	BuiltinTypeUint16,
	BuiltinTypeUint32,
	BuiltinTypeUint64,
	BuiltinTypeInt8,
	BuiltinTypeInt16,
	BuiltinTypeInt32,
	BuiltinTypeInt64,
	BuiltinTypeUnknownInt, /*used for integer literals*/
	BuiltinTypeFloat,
	BuiltinTypeVoid,
	BuiltinTypeBool,
	BuiltinTypeMsg,
	BuiltinTypeTaskid,
	BuiltinTypePinid
};

/* This enum has one element for each category of types, used as
   a tag in the Type struct */
enum {
	ArrayTypeTag,
	FunctionTypeTag,
	TaskTypeTag,
	BuiltinTypeTag,
	StructTypeTag,
	MessageTypeTag
};

/*
 * Here comes the structs and the functions related to them
 */

/* We start with typedefs for all struct type */
typedef struct AstNode AstNode;
typedef struct ProgNode ProgNode;
typedef struct DefineFunctionNode DefineFunctionNode;
typedef struct DefineStructNode DefineStructNode;
typedef struct DefineTaskNode DefineTaskNode;
typedef struct DefineMessageNode DefineMessageNode;
typedef struct MessageIdentifierNode MessageIdentifierNode;
typedef struct StructMemberNode StructMemberNode;
typedef struct ParameterNode ParameterNode;
typedef struct BuiltinTypeNode BuiltinTypeNode;
typedef struct StructTypeNode StructTypeNode;
typedef struct ArrayTypeNode ArrayTypeNode;
typedef struct WhileNode WhileNode;
typedef struct ForNode ForNode;
typedef struct SwitchNode SwitchNode;
typedef struct SwitchCaseNode SwitchCaseNode;
typedef struct IfNode IfNode;
typedef struct ElseIfNode ElseIfNode;
typedef struct ElseNode ElseNode;
typedef struct ReceiveNode ReceiveNode;
typedef struct ReceiveCaseNode ReceiveCaseNode;
typedef struct VarDeclNode VarDeclNode;
typedef struct BinaryOperationNode BinaryOperationNode;
typedef struct VariableLocationNode VariableLocationNode;
typedef struct StructLocationNode StructLocationNode;
typedef struct ArrayLocationNode ArrayLocationNode;
typedef struct UnaryOperationNode UnaryOperationNode;
typedef struct FunctionCallNode FunctionCallNode;
typedef struct AssignmentNode AssignmentNode;
typedef struct TernaryOperatorNode TernaryOperatorNode;
typedef struct IdentifierNode IdentifierNode;
typedef struct PinLiteralNode PinLiteralNode;
typedef struct IntLiteralNode IntLiteralNode;
typedef struct FloatLiteralNode FloatLiteralNode;
typedef struct BoolLiteralNode BoolLiteralNode;
typedef struct MessageLiteralNode MessageLiteralNode;
typedef struct ReturnNode ReturnNode;
typedef struct SpawnNode SpawnNode;
typedef struct SendNode SendNode;
typedef struct ExprStmtNode ExprStmtNode;
typedef struct Symbol Symbol;
typedef struct SymbolTable SymbolTable;
typedef struct Type Type;
typedef struct ArrayTypeDescriptor ArrayTypeDescriptor;
typedef struct FunctionTypeDescriptor FunctionTypeDescriptor;
typedef struct BuiltinTypeDescriptor BuiltinTypeDescriptor;
typedef struct StructTypeDescriptor StructTypeDescriptor;
typedef struct TaskTypeDescriptor TaskTypeDescriptor;
typedef struct MessageTypeDescriptor MessageTypeDescriptor;
typedef struct InitializeInfo InitializeInfo;
typedef struct StructInitializeInfo StructInitializeInfo;

/* Then comes the actual structs, and prototypes for important
   functions related to each of them. */
struct ProgNode {
	AstNode *toplevels;
	int spawnCount;
};
AstNode *mkProgNode(AstNode *);

struct DefineFunctionNode {
	AstNode *identifier;
	AstNode *parameters;
	AstNode *type;
	AstNode *statements;
};
AstNode *mkDefineFunctionNode(AstNode *, AstNode *, AstNode *, AstNode *);

struct DefineTaskNode {
	AstNode *identifier;
	AstNode *parameters;
	AstNode *statements;
};
AstNode *mkDefineTaskNode(AstNode *, AstNode *, AstNode *);

struct DefineStructNode {
	AstNode *identifier;
	AstNode *fields;
};
AstNode *mkDefineStructNode(AstNode *, AstNode *);

struct DefineMessageNode {
	AstNode *messagesIdentifiers;
};
AstNode *mkDefineMessageNode(AstNode *);

struct MessageIdentifierNode {
	AstNode *identifier;
	AstNode *parameters;
};
AstNode *mkMessageIdentifierNode(AstNode *, AstNode *);

struct StructMemberNode {
	AstNode *identifier;
	AstNode *type;
};
AstNode *mkStructMemberNode(AstNode *, AstNode *);

struct ParameterNode {
	AstNode *type;
	AstNode *identifier;
};
AstNode *mkParameterNode(AstNode *, AstNode *);

struct BuiltinTypeNode {
	int type;
};
AstNode *mkBuiltinTypeNode(int);

struct StructTypeNode {
	AstNode *identifier;
};
AstNode *mkStructTypeNode(AstNode *);

struct ArrayTypeNode {
	AstNode *elementType;
	AstNode *dimensions;
};
AstNode *mkArrayTypeNode(AstNode *, AstNode *);

struct WhileNode {
	AstNode *expression;
	AstNode *statements;
};
AstNode *mkWhileNode(AstNode *, AstNode *);

struct ForNode {
	AstNode *expressionInit;
	AstNode *expressionTest;
	AstNode *expressionUpdate; /* hehe */
	AstNode *statements;
};
AstNode *mkForNode(AstNode *, AstNode *, AstNode *, AstNode *);

struct SwitchNode {
	AstNode *expression;
	AstNode *cases;
};
AstNode *mkSwitchNode(AstNode *, AstNode *);

struct SwitchCaseNode {
	/* int eller bool, if null then default case hehe */
	AstNode *literal;
	AstNode *statements;
};
AstNode *mkSwitchCaseNode(AstNode *, AstNode *);

struct IfNode {
	AstNode *expression;
	AstNode *statements;
	/* else or elseif or neither */
	AstNode *elsePart;
};
AstNode *mkIfNode(AstNode *, AstNode *, AstNode *);

struct ElseIfNode {
	AstNode *expression;
	AstNode *statements;
	AstNode *elsePart;
};
AstNode *mkElseIfNode(AstNode *, AstNode *, AstNode *);

struct ElseNode {
	AstNode *statements;
};
AstNode *mkElseNode(AstNode *);

struct ReceiveNode {
	AstNode *cases;
};
AstNode *mkReceiveNode(AstNode *);

struct ReceiveCaseNode {
	/* if messagename is NULL, it is the default case */
	AstNode *messageName;
	AstNode *dataNames;
	AstNode *statements;
};
AstNode *mkReceiveCaseNode(AstNode *, AstNode *, AstNode *);

struct VarDeclNode {
	AstNode *type;
	AstNode *identifier;
	/* if expression is empty, then init to standard value */
	AstNode *expression;
	/* 1 if top level, else 0 */
	int toplevel;
};
AstNode *mkVarDeclNode(AstNode *, AstNode *, AstNode *, int);

struct BinaryOperationNode {
	AstNode *expression_left;
	/* enum */
	int operator;
	AstNode *expression_right;
};
AstNode *mkBinaryOperationNode(AstNode *, int, AstNode *);

struct VariableLocationNode {
	AstNode *identifier;
};
AstNode *mkVariableLocationNode(AstNode *);

struct StructLocationNode {
	AstNode *identifier;
	AstNode *location;
};
AstNode *mkStructLocationNode(AstNode *, AstNode *);

struct ArrayLocationNode {
	AstNode *identifier;
	AstNode *indicies;
};
AstNode *mkArrayLocationNode(AstNode *, AstNode *);

struct UnaryOperationNode {
	/* enum again */
	int operator;
	int fix; /* prefix or postfix */
	AstNode *expression;
};
AstNode *mkUnaryOperationNode(int, int, AstNode *);

struct FunctionCallNode {
	AstNode *identifier;
	AstNode *arguments;
};
AstNode *mkFunctionCallNode(AstNode *, AstNode *);

struct AssignmentNode {
	AstNode *location;
	AstNode *expression;
};
AstNode *mkAssignmentNode(AstNode *, AstNode *);

struct TernaryOperatorNode {
	AstNode *expressionTest;
	AstNode *expressionTrue;
	AstNode *expressionFalse;
};
AstNode *mkTernaryOperatorNode(AstNode *, AstNode *, AstNode *);

struct IdentifierNode {
	char *identifier;
	Symbol *symbol;
};
AstNode *mkIdentifierNode(char *);

struct IntLiteralNode {
	/* store its size here as well i think */
	long int value;
};
AstNode *mkPinLiteralNode(long int);

struct PinLiteralNode {
	long int value;
};
AstNode *mkIntLiteralNode(long int);

struct FloatLiteralNode {
	float value;
};
AstNode *mkFloatLiteralNode(float);

struct BoolLiteralNode {
	int value;
};
AstNode *mkBoolLiteralNode(int);

struct MessageLiteralNode {
	AstNode *identifier;
	AstNode *arguments;
};
AstNode *mkMessageLiteralNode(AstNode *, AstNode *);

struct ReturnNode {
	AstNode *expression; /* might be null for empty return*/
	Symbol *functionsym;
};
AstNode *mkReturnNode(AstNode *);

struct SpawnNode {
	AstNode *identifier;
	AstNode *arguments;
	char taskId;
};
AstNode *mkSpawnNode(AstNode *, AstNode *);

struct SendNode {
	AstNode *message;
	AstNode *receiver;
};
AstNode *mkSendNode(AstNode *, AstNode *);

struct ExprStmtNode {
	AstNode *expression;
};
AstNode *mkExprStmtNode(AstNode *);

struct AstNode {
	int tag;
	union {
		ProgNode Prog;
		DefineFunctionNode DefineFunction;
		DefineTaskNode DefineTask;
		DefineStructNode DefineStruct;
		DefineMessageNode DefineMessage;
		MessageIdentifierNode MessageIdentifier;
		StructMemberNode StructMember;
		ParameterNode Parameter;
		BuiltinTypeNode BuiltinType;
		StructTypeNode StructType;
		ArrayTypeNode ArrayType;
		WhileNode While;
		ForNode For;
		SwitchNode Switch;
		SwitchCaseNode SwitchCase;
		IfNode If;
		ElseIfNode ElseIf;
		ElseNode Else;
		ReceiveNode Receive;
		ReceiveCaseNode ReceiveCase;
		VarDeclNode VarDecl;
		BinaryOperationNode BinaryOperation;
		VariableLocationNode VariableLocation;
		StructLocationNode StructLocation;
		ArrayLocationNode ArrayLocation;
		UnaryOperationNode UnaryOperation;
		FunctionCallNode FunctionCall;
		AssignmentNode Assignment;
		TernaryOperatorNode TernaryOperator;
		IdentifierNode Identifier;
		PinLiteralNode PinLiteral;
		IntLiteralNode IntLiteral;
		FloatLiteralNode FloatLiteral;
		BoolLiteralNode BoolLiteral;
		MessageLiteralNode MessageLiteral;
		ReturnNode Return;
		SpawnNode Spawn;
		SendNode Send;
		ExprStmtNode ExprStmt;
	} node;
	int linenum;
	AstNode *next; /* The next element in the list */
	AstNode *chain; /* The next element in the chain of children */
	AstNode *children; /* The first child of this node */
	AstNode *parent; /* The parent node */
};
AstNode *append_node(AstNode *, AstNode *);
int nodeLength(AstNode *);

struct Symbol {
	char* name;
	Type *type;
	int globalvar;
	int used;
	int linenum; /* the line number where the symbol first appeared */
	Symbol* next;

	/* This is where the symbol is first found. */
	struct AstNode *first;

	/* Info about initialization of this symbol if it is a variable */
	InitializeInfo *initInfo;
};

struct SymbolTable {
	SymbolTable* previous;
	Symbol*	symbols;
};
void initializeSymbolTables(void);
void openScope(void);
void closeScope(void);
SymbolTable *getCurrentSymbolTable(void);
int insertSymbol(AstNode *, Type *);
Symbol* retrieveSymbol(AstNode *);
Symbol *retrieveSymbolFromTable(AstNode *, SymbolTable *);

struct ArrayTypeDescriptor {
	Type *elementType;
	AstNode *dimensions;
};
Type *mkArrayTypeDescriptor(Type *, AstNode *);

struct FunctionTypeDescriptor {
	int arity;
	Type **parameterTypes;
	Type *returnType;
};
Type *mkFunctionTypeDescriptor(int, Type **, Type *);

struct TaskTypeDescriptor {
	int arity;
	Type **parameterTypes;
};
Type *mkTaskTypeDescriptor(int, Type **);

struct BuiltinTypeDescriptor {
	int builtinType;
};
Type *mkBuiltinTypeDescriptor(int);

struct StructTypeDescriptor {
	char *name;
	SymbolTable *fields;
};
Type *mkStructTypeDescriptor(char *name, SymbolTable *);

struct MessageTypeDescriptor {
	int arity;
	Type **parameterTypes;
};
Type *mkMessageTypeDescriptor(int, Type**);

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
AstNode *getDefaultValue(Type *type);

struct StructInitializeInfo {
	char *fieldname;
	Type *fieldtype;
	InitializeInfo *info;
	StructInitializeInfo *next;
};

struct InitializeInfo {
	int varInitialized;
	InitializeInfo **arrayInitialized;
	StructInitializeInfo *structInitialized;
};
int isInitialized(InitializeInfo *, Type *);
void setInitialized(InitializeInfo *, Type *);
void setInitializedStructField(InitializeInfo *, char *);