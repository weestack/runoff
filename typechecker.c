#include <stdio.h>
#include <stdlib.h>

#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

void typeCheckNode(AstNode *);
Type *typeof(AstNode *);
int buildinTypeMatch(Type *, int);
int buildinTypeMatchInt(Type *);
int numericType(Type *);
void printTypeFail(char *, AstNode *, Type *);
char *typeString(Type *);
Type *binaryOperatorType(AstNode *);
Type *unaryOperatorType(AstNode *);


static int errors;
extern char* filename;

int typeCheck(AstNode *tree){
    typeCheckNode(tree);
    return errors;
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
        if(!buildinTypeMatch(type, builtintype_bool))
            printTypeFail("While loop expected bool", node->node.While.expression, type);

        break;
	case For:
		if(node->node.For.expressionTest == NULL){
			break;
		}		
        type = typeof(node->node.For.expressionTest);
        if(!buildinTypeMatch(type, builtintype_bool))
            printTypeFail("For loop test expected bool", node->node.For.expressionTest, type);

        break;
	case Switch:
        type = typeof(node->node.Switch.expression);
        if(!buildinTypeMatchInt(type))
            printTypeFail("Switch expression expected int", node->node.Switch.expression, type);

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
        typeof(node);
        break;
	case VariableLocation:
        break;
	case StructLocation:
        break;
	case ArrayLocation:
        break;
	case UnaryOperation:
		typeof(node);
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
    AstNode *id;
	if(node == NULL){
		return NULL;
	}
    switch(node->tag){
        case ArrayLocation:
            id = node->node.ArrayLocation.identifier;
            return id->node.Identifier.symbol->type;
        case StructLocation:
            id = node->node.StructLocation.identifier;
            return id->node.Identifier.symbol->type;
        case VariableLocation:
            id = node->node.VariableLocation.identifier;
            return id->node.Identifier.symbol->type;
		case Identifier:
			return node->node.Identifier.symbol->type;
        case IntLiteral: /* We'll be back! */
			return mkBuiltinTypeDiscriptor(builtintype_unknownInt);
        case FloatLiteral:
            return mkBuiltinTypeDiscriptor(builtintype_float);
        case BoolLiteral:
            return mkBuiltinTypeDiscriptor(builtintype_bool);
        case BinaryOperation:
            return binaryOperatorType(node);
        case UnaryOperation:
			return unaryOperatorType(node);
        case FunctionCall:
            id = node->node.FunctionCall.identifier;
			return id->node.Identifier.symbol->type->tags.typeFunction.returnType;
        case Assignment:
            return typeof(node->node.Assignment.location);
        case VarDecl:
            id = node->node.VarDecl.identifier;
			return id->node.Identifier.symbol->type;
        case TernaryOperator:
			return typeof(node->node.TernaryOperator.expressionTrue);
        case Spawn:
            return mkBuiltinTypeDiscriptor(builtintype_taskid);
        default:
            return NULL;
    }
    return NULL;
}

int buildinTypeMatch(Type *a, int b){
	Type *bType = mkBuiltinTypeDiscriptor(b); 
    if(a == NULL)
        return 0;

    if(a->tag == BuiltinTypeTag){
		if (a->tags.typeBuiltin.builtinType == b){
			return 1;
		}
		else if(a->tags.typeBuiltin.builtinType == builtintype_unknownInt && buildinTypeMatchInt(bType)){
			return 1;
		}
		else if (b == builtintype_unknownInt && buildinTypeMatchInt(a)){
			return 1;
		}
		return 0;		
	}
	else
		return 0;
}

int buildinTypeMatchInt(Type *a){
    if(a == NULL)
        return 0;

    if(a->tag != BuiltinTypeTag)
        return 0;

    switch(a->tags.typeBuiltin.builtinType){
    case builtintype_uint8: /* Fall through */
	case builtintype_uint16: /* Fall through */
	case builtintype_uint32: /* Fall through */
	case builtintype_uint64: /* Fall through */
	case builtintype_int8: /* Fall through */
	case builtintype_int16: /* Fall through */
	case builtintype_int32: /* Fall through */
	case builtintype_int64: /* Fall through */
	case builtintype_int:
        return 1;
    default:
        return 0;
    }
}

int numericType(Type *type){
	if(type == NULL)
		return 0;
	if(type->tag != BuiltinTypeTag)
		return 0;

	if(buildinTypeMatchInt(type))
		return 1;

	if(type->tags.typeBuiltin.builtinType == builtintype_float)
		return 1;

	if(type->tags.typeBuiltin.builtinType == builtintype_unknownInt)
		return 1;

	return 0;
}

void printTypeFail(char *fail_message, AstNode *node, Type *type){
    char *text_type = typeString(type);
    errors++;
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
        return smprintf("%s", builtintypeNames[type->tags.typeBuiltin.builtinType]);
	case StructTypeTag:
        return smprintf("Struct Type");
	case MessageTypeTag:
        return smprintf("Message Type");
    }
    return smprintf("undefined");
}

Type *binaryOperatorType(AstNode *node){
    Type *left = typeof(node->node.BinaryOperation.expression_left);
    Type *right = typeof(node->node.BinaryOperation.expression_right);
    int expected_right = 0, expected_left = 0, return_type = 0;
    char *other_error = NULL;

    switch(node->node.BinaryOperation.operator){
    case elogical_and: /*Fall through*/
    case elogical_or:  
        expected_left = expected_right = return_type = builtintype_bool;
        break;
    case esmaller_equal: /*Fall through*/
	case egreater_equal: /*Fall through*/
    case esmaller_than:  /*Fall through*/
    case ebigger_than:
        return_type = builtintype_bool;
        if(!numericType(left)){
            other_error = "Expected numeric types";
			break;
        }
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
        break;
	case eequal: /*Fall through*/
	case enot_equal:
		return_type = builtintype_bool;
		if(left == NULL || left->tag != BuiltinTypeTag){
			other_error = "Expected builtin types";
			break;
		}
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
        break;
	case eplus: /*Fall through*/
	case eminus:/*Fall through*/
	case etimes:/*Fall through*/
	case edivid:
        if(!numericType(left)){
            other_error = "Expected numeric types";
			break;
        }
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
		return_type = expected_right;
        break;
	case emod:         /*Fall through*/
	case ebit_and:     /*Fall through*/
	case ebit_or:      /*Fall through*/
	case eright_shift: /*Fall through*/
	case eleft_shift:  /*Fall through*/
	case ebit_xor:
		if(!buildinTypeMatchInt(left)){
        	other_error = "Expected integer types";
			break;
		}

		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
		return_type = expected_right;
        break;
    default:
        return NULL;
    }

    if(other_error != NULL){
        char *text_type_right = typeString(right);
        char *text_type_left = typeString(left);
        errors++;
        printf("%s:%d: binary operator %s %s.\n\tActual type for left: %s\n\tActual type for right: %s \n",
            filename, node->linenum,
            operatorNames[node->node.BinaryOperation.operator], other_error,
            text_type_left, text_type_right);

        free(text_type_left);
        free(text_type_right);
        return NULL;
    }

    if(!buildinTypeMatch(left, expected_left)){
        char *fail_message = smprintf("left hand side of %s expected type %s",
            operatorNames[node->node.BinaryOperation.operator],
            builtintypeNames[expected_left]);

        printTypeFail(fail_message, node->node.BinaryOperation.expression_left, left);
        free(fail_message);
        
        return NULL;
    } else if(!buildinTypeMatch(right, expected_right)){
        char *fail_message = smprintf("right hand side of %s expected type %s",
            operatorNames[node->node.BinaryOperation.operator],
            builtintypeNames[expected_right]);
        printTypeFail(fail_message, node->node.BinaryOperation.expression_right, right);
        free(fail_message);
        return NULL;
    }
    return mkBuiltinTypeDiscriptor(return_type);

}

Type *unaryOperatorType(AstNode *node){
	Type *operandType = typeof(node->node.UnaryOperation.expression);
	char *otherError = NULL;
	int expected = 0, returnType = 0;
	switch (node->node.UnaryOperation.operator){
	case elogical_not:
		expected = builtintype_bool;
		returnType = expected;
		break;
	case edecrement: /*Fall through*/
	case eincrement: /*Fall through*/
	case ebit_not:
		if(!buildinTypeMatchInt(operandType)){
			otherError = "Expected integer type";
			break;
		}
		expected = operandType->tags.typeBuiltin.builtinType;
		returnType = expected;
		break;
	
	default:
		return NULL;
	}

	if (otherError != NULL){
		char* errorMessage = smprintf("%s %s", operatorNames[node->node.UnaryOperation.operator], otherError);
		printTypeFail(errorMessage, node->node.UnaryOperation.expression, operandType);
		free(errorMessage);
		return NULL;
	}
	if (!buildinTypeMatch(operandType, expected)){
		char *errorMessage = smprintf("%s expected operand of type %s", operatorNames[node->node.UnaryOperation.operator], builtintypeNames[expected]);
		printTypeFail(errorMessage, node->node.UnaryOperation.expression, operandType);
		free(errorMessage);
		return NULL;
	}
	return mkBuiltinTypeDiscriptor(returnType);	
}