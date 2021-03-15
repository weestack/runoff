typedef struct AstNode AstNode;

typedef struct ProgNode ProgNode;
typedef struct DefineConstNode DefineConstNode;
typedef struct DefineFunctionNode DefineFunctionNode;
typedef struct DefineTaskNode DefineTaskNode;
typedef struct DefineMessageNode DefineMessageNode;
typedef struct IncludeRunoffFileNode IncludeRunoffFileNode;
typedef struct MessageIdentifierNode MessageIdentifierNode;
typedef struct StructMemberNode StructMemberNode;
typedef struct ParameterNode ParameterNode;
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


struct ProgNode {
	AstNode *toplevels;
};


struct DefineConstNode {
	AstNode *identifier;
	AstNode *int_literal;
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


struct StructTypeNode {
	AstNode *identifier;
};


struct ArrayTypeNode {
	AstNode *type;
	/* if int literal is null, then its eq to int a[]; */
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
	AstNode *messageName;
	AstNode *dataNames;
	AstNode *statements;
};


struct VarDeclNode {
	AstNode *identifier;
	/* if expression is empty, then init to standard value */
	AstNode *expression;
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


struct AstNode {
	int tag;
	union {
		ProgNode Prog;
		DefineConstNode DefineConst;
		DefineFunctionNode DefineFunction;
		DefineTaskNode DefineTask;
		DefineMessageNode DefineMessage;
		IncludeRunoffFileNode IncludeRunoffFile;
		MessageIdentifierNode MessageIdentifier;
		StructMemberNode StructMember;
		ParameterNode Parameter;
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
	} node;
	AstNode *next;
};


AstNode *mkProgNode(AstNode *toplevels);
AstNode *mkDefineConstNode(AstNode *identifier, AstNode *int_literal);
AstNode *mkDefineFunctionNode(AstNode *identifier, AstNode *parameters, AstNode *type, AstNode *statements);
AstNode *mkDefineTaskNode(AstNode *identifier, AstNode *parameters, AstNode *statements);
AstNode *mkDefineMessageNode(AstNode *messagesIdentifiers);
AstNode *mkIncludeRunoffFileNode(AstNode *identifier);
AstNode *mkMessageIdentifierNode(AstNode *identifier, AstNode *parameters);
AstNode *mkStructMemberNode(AstNode *identifier, AstNode *type);
AstNode *mkParameterNode(AstNode *type, AstNode *identifier);
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
AstNode *mkVarDeclNode(AstNode *identifier, AstNode *expression);
AstNode *mkBinaryOperationNode(AstNode *expression_left, int operator, AstNode *expression_right);
AstNode *mkVariableLocationNode(AstNode *identifier);
AstNode *mkStructLocationNode(AstNode *identifier, AstNode *location);
AstNode *mkArrayLocationNode(AstNode *identifier, AstNode *indicies);
AstNode *mkUnaryOperationNode(int operator, int fix, AstNode *expression);
AstNode *mkFunctionCallNode(AstNode *identifier, AstNode *arguments);
AstNode *mkAssignmentNode(AstNode *location, AstNode *expression);
AstNode *mkTernaryOperatorNode(AstNode *expressionTest, AstNode *expressionTrue, AstNode *expressionFalse);

enum NodeTypes{
	Prog,
	DefineConst,
	DefineFunction,
	DefineTask,
	DefineMessage,
	IncludeRunoffFile,
	MessageIdentifier,
	StructMember,
	Parameter,
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
	TernaryOperator
};

enum operatore {
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

enum {
	prefix,
	postfix
};
