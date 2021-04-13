typedef struct AstNode AstNode;
typedef struct ProgNode ProgNode;
typedef struct DefineFunctionNode DefineFunctionNode;
typedef struct DefineStructNode DefineStructNode;
typedef struct DefineTaskNode DefineTaskNode;
typedef struct DefineMessageNode DefineMessageNode;
typedef struct IncludeRunoffFileNode IncludeRunoffFileNode;
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

struct ProgNode {
	AstNode *toplevels;
};

struct DefineFunctionNode {
	AstNode *identifier;
	AstNode *parameters;
	AstNode *type;
	AstNode *statements;
};

struct DefineTaskNode {
	AstNode *identifier;
	AstNode *parameters;
	AstNode *statements;
};

struct DefineStructNode {
	AstNode *identifier;
	AstNode *fields;
};


struct DefineMessageNode {
	AstNode *messagesIdentifiers;
};

struct IncludeRunoffFileNode {
	AstNode *identifier;
};

struct MessageIdentifierNode {
	AstNode *identifier;
	AstNode *parameters;
};

struct StructMemberNode {
	AstNode *identifier;
	AstNode *type;
};

struct ParameterNode {
	AstNode *type;
	AstNode *identifier;
};

struct BuiltinTypeNode {
	int type;
};

struct StructTypeNode {
	AstNode *identifier;
};

struct ArrayTypeNode {
	AstNode *type;
	AstNode *int_literal;
};

struct WhileNode {
	AstNode *expression;
	AstNode *statements;
};

struct ForNode {
	AstNode *expressionInit;
	AstNode *expressionTest;
	AstNode *expressionUpdate; /* hehe */
	AstNode *statements;
};

struct SwitchNode {
	AstNode *expression;
	AstNode *cases;
};

struct SwitchCaseNode {
	/* int eller bool, if null then default case hehe */
	AstNode *literal;
	AstNode *statements;
};

struct IfNode {
	AstNode *expression;
	AstNode *statements;
	/* else or elseif or neither */
	AstNode *elsePart;
};

struct ElseIfNode {
	AstNode *expression;
	AstNode *statements;
	AstNode *elsePart;
};

struct ElseNode {
	AstNode *statements;
};

struct ReceiveNode {
	AstNode *cases;
};

struct ReceiveCaseNode {
	/* if messagename is NULL, it is the default case */
	AstNode *messageName;
	AstNode *dataNames;
	AstNode *statements;
};

struct VarDeclNode {
	AstNode *type;
	AstNode *identifier;
	/* if expression is empty, then init to standard value */
	AstNode *expression;
	/* 1 if top level, else 0 */
	int toplevel;
};

struct BinaryOperationNode {
	AstNode *expression_left;
	/* enum */
	int operator;
	AstNode *expression_right;
};

struct VariableLocationNode {
	AstNode *identifier;
};

struct StructLocationNode {
	AstNode *identifier;
	AstNode *location;
};

struct ArrayLocationNode {
	AstNode *identifier;
	AstNode *indicies;
};

struct UnaryOperationNode {
	/* enum again */
	int operator;
	int fix; /* prefix or postfix */
	AstNode *expression;
};

struct FunctionCallNode {
	AstNode *identifier;
	AstNode *arguments;
};

struct AssignmentNode {
	AstNode *location;
	AstNode *expression;
};

struct TernaryOperatorNode {
	AstNode *expressionTest;
	AstNode *expressionTrue;
	AstNode *expressionFalse;
};

struct IdentifierNode {
	char *identifier;
	Symbol *symbol;
};

struct IntLiteralNode {
	/* store its size here as well i think */
	long int value;
};

struct PinLiteralNode {
	long int value;
};

struct FloatLiteralNode {
	float value;
};

struct BoolLiteralNode {
	int value;
};

struct MessageLiteralNode {
	AstNode *identifier;
	AstNode *arguments;
};

struct ReturnNode {
	AstNode *expression; /* might be null for empty return*/
	 /* the symbol of the wrapping function. Note that it has
	    to be a pointer to a pointer, since the return nodes are handled via copies,
	    so modifications to them doesn't affect the original. */
	Symbol **functionsym;
};

struct SpawnNode {
	AstNode *identifier;
	AstNode *arguments;
};

struct SendNode {
	AstNode *message;
	AstNode *receiver;
};

struct ExprStmtNode {
	AstNode *expression;
};

struct AstNode {
	int tag;
	union {
		ProgNode Prog;
		DefineFunctionNode DefineFunction;
		DefineTaskNode DefineTask;

		DefineStructNode DefineStruct;
		DefineMessageNode DefineMessage;
		IncludeRunoffFileNode IncludeRunoffFile;
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
	AstNode *next;
};

AstNode *mkProgNode(AstNode *toplevels);
AstNode *mkDefineFunctionNode(AstNode *identifier, AstNode *parameters, AstNode *type, AstNode *statements);
AstNode *mkDefineTaskNode(AstNode *identifier, AstNode *parameters, AstNode *statements);
AstNode *mkDefineStructNode(AstNode *identifier, AstNode *fields);
AstNode *mkDefineMessageNode(AstNode *messagesIdentifiers);
AstNode *mkIncludeRunoffFileNode(AstNode *identifier);
AstNode *mkMessageIdentifierNode(AstNode *identifier, AstNode *parameters);
AstNode *mkStructMemberNode(AstNode *identifier, AstNode *type);
AstNode *mkParameterNode(AstNode *type, AstNode *identifier);
AstNode *mkBuiltinTypeNode(int type);
AstNode *mkStructTypeNode(AstNode *identifier);
AstNode *mkArrayTypeNode(AstNode *type, AstNode *int_literal);
AstNode *mkWhileNode(AstNode *expression, AstNode *statements);
AstNode *mkForNode(AstNode *expressionInit, AstNode *expressionTest, AstNode *expressionUpdate, AstNode *statements);
AstNode *mkSwitchNode(AstNode *expression, AstNode *cases);
AstNode *mkSwitchCaseNode(AstNode *literal, AstNode *statements);
AstNode *mkIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart);
AstNode *mkElseIfNode(AstNode *expression, AstNode *statements, AstNode *elsePart);
AstNode *mkElseNode(AstNode *statements);
AstNode *mkReceiveNode(AstNode *cases);
AstNode *mkReceiveCaseNode(AstNode *messageName, AstNode *dataNames, AstNode *statements);
AstNode *mkVarDeclNode(AstNode *type, AstNode *identifier, AstNode *expression, int toplevel);
AstNode *mkBinaryOperationNode(AstNode *expression_left, int operator, AstNode *expression_right);
AstNode *mkVariableLocationNode(AstNode *identifier);
AstNode *mkStructLocationNode(AstNode *identifier, AstNode *location);
AstNode *mkArrayLocationNode(AstNode *identifier, AstNode *indicies);
AstNode *mkUnaryOperationNode(int operator, int fix, AstNode *expression);
AstNode *mkFunctionCallNode(AstNode *identifier, AstNode *arguments);
AstNode *mkAssignmentNode(AstNode *location, AstNode *expression);
AstNode *mkTernaryOperatorNode(AstNode *expressionTest, AstNode *expressionTrue, AstNode *expressionFalse);
AstNode *mkIdentifierNode(char *identifier);
AstNode *mkPinLiteralNode(long int value);
AstNode *mkIntLiteralNode(long int value);
AstNode *mkFloatLiteralNode(float value);
AstNode *mkBoolLiteralNode(int value);
AstNode *mkMessageLiteralNode(AstNode *identifier, AstNode *arguments);
AstNode *mkReturnNode(AstNode *expression);
AstNode *mkSpawnNode(AstNode *identifier, AstNode *arguments);
AstNode *mkSendNode(AstNode *message, AstNode *receiver);
AstNode *mkExprStmtNode(AstNode *expression);

AstNode *append_node(AstNode* siblingA, AstNode* siblingB);
AstNode *concat_node(AstNode *siblingA, AstNode* siblingB);
int nodeLength(AstNode *);
AstNode *getChildren(AstNode *node);

enum NodeTypes {
	Prog,
	DefineFunction,
	DefineTask,
	DefineStruct,
	DefineMessage,
	IncludeRunoffFile,
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

enum operators {
	elogical_and,
	elogical_or,
	egreater_equal,
	esmaller_equal,
	eequal,
	enot_equal,
	edecrement,
	eincrement,
	esmaller_than,
	ebigger_than,
	emod,
	eplus,
	eminus,
	etimes,
	edivid,
	elogical_not,
	ebit_and,
	ebit_or,
	eright_shift,
	eleft_shift,
	ebit_xor,
	ebit_not
};

enum fixity {
	prefix,
	postfix
};

enum builtintypes {
	builtintype_uint8,
	builtintype_uint16,
	builtintype_uint32,
	builtintype_uint64,
	builtintype_int8,
	builtintype_int16,
	builtintype_int32,
	builtintype_int64,
	builtintype_unknownInt, /*used for integer literals*/
	builtintype_float,
	builtintype_void,
	builtintype_bool,
	builtintype_msg,
	builtintype_taskid,
	builtintype_pinid
};


/* Symbol table stuff */
int insertSymbol(AstNode *, Type *);
Symbol* retrieveSymbol(AstNode *);
Symbol *retrieveSymbolFromTable(AstNode *node, SymbolTable *table);
