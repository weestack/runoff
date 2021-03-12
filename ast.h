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
    /*magic_word*/
    AstNode *identifier;
    AstNode *int_literal;
};


struct DefineFunctionNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *parameters;
    AstNode *type;
    AstNode *statements;
};


struct DefineTaskNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *parameters;
    AstNode *statements;
};


struct DefineMessageNode {
    /*magic_word*/
    AstNode *messagesIdentifiers;
};


struct IncludeRunoffFileNode {
    /*magic_word*/
    AstNode *identifier;
};


struct MessageIdentifierNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *parameters;
};


struct StructMemberNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *type;
};


struct ParameterNode {
    /*magic_word*/
    AstNode *type;
    AstNode *identifier;
};


struct StructTypeNode {
    /*magic_word*/
    AstNode *identifier;
};


struct ArrayTypeNode {
    /*magic_word*/
    AstNode *type;
    /* if int literal is null, then its eq to int a[]; */
    AstNode *int_literal;
};



struct WhileNode {
    /*magic_word*/
    AstNode *expression;
    AstNode *statements;
};


struct ForNode {
    /*magic_word*/
    AstNode *expressionInit;
    AstNode *expressionTest;
    AstNode *expressionUpdate; /* hehe */
    AstNode *statements;
};


struct SwitchNode {
    /*magic_word*/
    AstNode *expression;
    AstNode *cases;
};


struct SwitchCaseNode {
    /*magic_word*/
    /* int eller bool, if null then default case hehe */
    AstNode *literal;
    AstNode *statements;
};


struct IfNode {
    /*magic_word*/
    AstNode *expression;
    AstNode *statements;
    /* else or elseif or neither */
    AstNode *elsePart;
};


struct ElseIfNode {
    /*magic_word*/
    AstNode *expression;
    AstNode *statements;
    AstNode *elsePart;
};


struct ElseNode {
    /*magic_word*/
    AstNode *statements;
};


struct ReceiveNode {
    /*magic_word*/
    AstNode *cases;
};


struct ReceiveCaseNode {
    /*magic_word*/
    AstNode *messageName;
    AstNode *dataNames;
    AstNode *statements;
};


struct VarDeclNode {
    /*magic_word*/
    AstNode *identifier;
    /* if expression is empty, then init to standard value */
    AstNode *expression;
};


struct BinaryOperationNode {
    /*magic_word*/
    AstNode *expression_left;
    /* enum */
    int operator;
    AstNode *expression_right;
};


struct VariableLocationNode {
    /*magic_word*/
    AstNode *identifier;
};


struct StructLocationNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *location;
};


struct ArrayLocationNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *indicies;
};


struct UnaryOperationNode {
    /*magic_word*/
    /* enum again */
    int operator;
    AstNode *expression;
};


struct FunctionCallNode {
    /*magic_word*/
    AstNode *identifier;
    AstNode *arguments;
};


struct AssignmentNode {
    /*magic_word*/
    AstNode *location;
    AstNode *expression;
};


struct TernaryOperatorNode {
    /*magic_word*/
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


AstNode *mkProgNode();
AstNode *mkDefineConstNode();
AstNode *mkDefineFunctionNode();
AstNode *mkDefineTaskNode();
AstNode *mkDefineMessageNode();
AstNode *mkIncludeRunoffFileNode();
AstNode *mkMessageIdentifierNode();
AstNode *mkStructMemberNode();
AstNode *mkParameterNode();
AstNode *mkStructTypeNode();
AstNode *mkArrayTypeNode();
AstNode *mkWhileNode();
AstNode *mkForNode();
AstNode *mkSwitchNode();
AstNode *mkSwitchCaseNode();
AstNode *mkIfNode();
AstNode *mkElseIfNode();
AstNode *mkElseNode();
AstNode *mkReceiveNode();
AstNode *mkReceiveCaseNode();
AstNode *mkVarDeclNode();
AstNode *mkBinaryOperationNode();
AstNode *mkVariableLocationNode();
AstNode *mkStructLocationNode();
AstNode *mkArrayLocationNode();
AstNode *mkUnaryOperationNode();
AstNode *mkFunctionCallNode();
AstNode *mkAssignmentNode();
AstNode *mkTernaryOperatorNode();

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
