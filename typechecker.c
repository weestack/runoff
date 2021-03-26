#include <stdio.h>
#include <stdlib.h>

#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

void typeCheckNode(AstNode *node);
Type *typeof(AstNode *node);
int buildinTypeMatch(Type *a, int b);
void printTypeFail(char *, AstNode *, Type *type);
char *typeString(Type *type);

static int errors;
extern char* filename;

int typeCheck(AstNode *tree){
    typeCheckNode(tree);
    return 0;
}

void typeCheckNode(AstNode *node){
    Type *type;
    AstNode *children;
    
    switch(node->tag){
    case Prog: /* Nothing */
        break;
	case DefineFunction: /* Nothing */
        break;
	case DefineTask: /* Nothing */
        break;
	case DefineStruct: /* Nothing */
        break;
	case DefineMessage: /* Nothing */
        break;
	case IncludeRunoffFile: /* Nothing */
        break;
	case MessageIdentifier: /* Nothing */
        break;
	case StructMember: /* Nothing */
        break;
	case Parameter: /* Nothing */
        break;
	case BuiltinType: /* Nothing */
        break;
	case StructType: /* Nothing */
        break;
	case ArrayType: /* Nothing */
        break;
	case While: 
        type = typeof(node->node.While.expression);
        if(!buildinTypeMatch(type, builtintype_bool)){
            errors++;
            printTypeFail("While loop expected bool", node->node.While.expression, type);
        }
        break;
	case For:
        break;
	case Switch:
        break;
	case SwitchCase:
        break;
	case If:
        break;
	case ElseIf:
        break;
	case Else:
        break;
	case Receive:
        break;
	case ReceiveCase:
        break;
	case VarDecl:
        break;
	case BinaryOperation:
        break;
	case VariableLocation:
        break;
	case StructLocation:
        break;
	case ArrayLocation:
        break;
	case UnaryOperation:
        break;
	case FunctionCall:
        break;
	case Assignment:
        break;
	case TernaryOperator:
        break;
	case Identifier:
        break;
	case IntLiteral:
        break;
	case FloatLiteral:
        break;
	case BoolLiteral:
        break;
	case Return:
        break;
	case Spawn:
        break;
	case Send:
        break;
	case ExprStmt:
        break;    
    }

    children = getChildren(node);
    for(; children!=NULL; children=children->next)
        typeCheckNode(children);
}

Type *typeof(AstNode *node){
    switch(node->tag){
        case ArrayLocation:
            break;
        case StructLocation:
            break;
        case VariableLocation:
            break;
        case IntLiteral:
            break;
        case FloatLiteral:
            break;
        case BoolLiteral:
            break;
        case BinaryOperation:
            break;
        case UnaryOperation:
            break;
        case FunctionCall:
            break;
        case Assignment:
            break;
        case VarDecl:
            break;
        case TernaryOperator:
            break;
        case Spawn:
            break;
        default:
            return NULL;
    }
    return NULL;
}

int buildinTypeMatch(Type *a, int b){
    if(a == NULL){
        return 0;
    }
    if(a->tag == BuiltinTypeTag && a->tags.typeBuiltin.builtinType == b)
        return 1;
    else
        return 0;
}

void printTypeFail(char *fail_message, AstNode *node, Type *type){
    char *text_type = typeString(type);
    printf("%s:%d: %s. Actual type: %s \n", filename, node->linenum, fail_message, text_type);
    free(text_type);
}

char *typeString(Type *type){
    if(type == NULL)
        return smprintf("undefined");

    switch(type->tag){
    case ArrayTypeTag:
        return smprintf("Array Type");
	case FunctionTypeTag:
        return smprintf("Function Type");
	case TaskTypeTag:
        return smprintf("Task Type");
	case BuiltinTypeTag:
        return smprintf("Builtin Type");
	case StructTypeTag:
        return smprintf("Struct Type");
	case MessageTypeTag:
        return smprintf("Message Type");
    }
    return smprintf("undefined");
}