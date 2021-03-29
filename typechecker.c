#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int typeMatch(Type *, Type *);
void checkFunctionCall(AstNode *node);
void checkSpawnNode(AstNode *node);


static int errors;
extern char* filename;

int typeCheck(AstNode *tree){
    typeCheckNode(tree);
    return errors;
}

void typeCheckNode(AstNode *node){
    Type *typeA, *typeB, *typeC;
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
        typeA = typeof(node->node.While.expression);
        if(!buildinTypeMatch(typeA, builtintype_bool))
            printTypeFail("While loop expected bool", node->node.While.expression, typeA);

        break;
	case For:
		if(node->node.For.expressionTest == NULL){
			break;
		}
        typeA = typeof(node->node.For.expressionTest);
        if(!buildinTypeMatch(typeA, builtintype_bool))
            printTypeFail("For loop test expected bool", node->node.For.expressionTest, typeA);

        break;
	case Switch:
        typeA = typeof(node->node.Switch.expression);
        if(!buildinTypeMatchInt(typeA))
            printTypeFail("Switch expression expected int", node->node.Switch.expression, typeA);

        break;
	case SwitchCase: /*Nothing*/
        break;
	case If:
		typeA = typeof(node->node.If.expression);
        if(!buildinTypeMatch(typeA, builtintype_bool))
            printTypeFail("if statement expected bool", node->node.If.expression, typeA);

        break;
	case ElseIf:
		typeA = typeof(node->node.ElseIf.expression);
        if(!buildinTypeMatch(typeA, builtintype_bool))
            printTypeFail("if statement expected bool", node->node.ElseIf.expression, typeA);
        break;
	case Else: /*Nothing*/
        break;
	case Receive: /*Nothing*/
        break;
	case ReceiveCase: /*Nothing*/
        break;
	case VarDecl:
		if(node->node.VarDecl.expression == NULL)
			break;

		typeA = typeof(node->node.VarDecl.identifier);
		typeB = typeof(node->node.VarDecl.expression);
		if (!typeMatch(typeA, typeB)){
			char* typeAString = typeString(typeA);
			char* errorMessage = smprintf("incompatible types expected %s", typeAString);
			printTypeFail(errorMessage, node->node.VarDecl.expression, typeB);
			free(typeAString);
			free(errorMessage);
		}
        break;
	case VariableLocation: /*Nothing*/
        break;
	case StructLocation: /*Nothing*/
        break;
	case ArrayLocation: /*Nothing*/
        break;
	case FunctionCall:
		checkFunctionCall(node);
        break;
	case Assignment:
		typeA = typeof(node->node.Assignment.location);
		typeB = typeof(node->node.Assignment.expression);
		if(!typeMatch(typeA, typeB)){
			char* typeAString = typeString(typeA);
			char* errorMessage = smprintf("incompatible types expected %s", typeAString);
			printTypeFail(errorMessage, node->node.Assignment.location, typeB);
			free(typeAString);
			free(errorMessage);
		}
        break;
	case TernaryOperator:
		typeA = typeof(node->node.TernaryOperator.expressionTest);
		typeB = typeof(node->node.TernaryOperator.expressionTrue);
		typeC = typeof(node->node.TernaryOperator.expressionFalse);
		if(!typeMatch(typeB, typeC)){
			char* typeBString = typeString(typeB);
			char* errorMessage = smprintf("incompatible types expected %s", typeBString);
			printTypeFail(errorMessage, node->node.TernaryOperator.expressionTrue, typeC);
			free(typeBString);
			free(errorMessage);
		}
		if(!buildinTypeMatch(typeA, builtintype_bool)){
			printTypeFail("ternary operator test must be boolean", node->node.TernaryOperator.expressionTest, typeA);
		}
        break;
	case Identifier: /*Nothing*/
        break;
    case PinLiteral: /*Nothing*/
        break;
	case IntLiteral: /*Nothing*/
        break;
	case FloatLiteral: /*Nothing*/
        break;
	case BoolLiteral: /*Nothing*/
        break;
    case MessageLiteral: /* a bit like function call. Check the args match the "prototype" */
        break;
    case Return: /* should check that the expression type matches the return type of the enclosing function */
        break;
	case Spawn:
		checkSpawnNode(node);
        break;
	case Send:
		typeA = typeof(node->node.Send.message);
		typeB = typeof(node->node.Send.receiver);
		if(!buildinTypeMatch(typeA, builtintype_msg))
			printTypeFail("body of send should be a message", node->node.Send.message, typeA);
		if(!buildinTypeMatch(typeB, builtintype_taskid))
			printTypeFail("receiver of send should be a taskid", node->node.Send.receiver, typeB);
        break;
    case BinaryOperation: /* Fall through */
    case UnaryOperation: /* Fall through */
	case ExprStmt:
		typeof(node); /* typeof will check that the expression is ok (it tries to find its type) */
        break;
    }

    children = getChildren(node);
    for(; children!=NULL; children=children->next)
        typeCheckNode(children);
}

Type *typeof(AstNode *node){
    AstNode *id;
    Type *tmp;
    AstNode *index;

	if(node == NULL){
		return NULL;
	}
    switch(node->tag){
        case ArrayLocation:
            id = node->node.ArrayLocation.identifier;
            index = node->node.ArrayLocation.indicies;
            for(tmp = id->node.Identifier.symbol->type;
            	index != NULL;
            	tmp = tmp->tags.typeArray.elementType, index = index->next){

            	if(tmp->tag != ArrayTypeTag){
            		errors++;
            		printf("%s:%d: Index in array %s too deep\n", filename, id->linenum, id->node.Identifier.identifier);
            		return NULL;
            	}
            }
            return tmp;
        case StructLocation:
            return typeof(node->node.StructLocation.location);
        case VariableLocation:
            id = node->node.VariableLocation.identifier;
            return id->node.Identifier.symbol->type;
		case Identifier:
			return node->node.Identifier.symbol->type;
		case PinLiteral:
			return mkBuiltinTypeDiscriptor(builtintype_pinid);
        case IntLiteral:
			return mkBuiltinTypeDiscriptor(builtintype_unknownInt);
        case FloatLiteral:
            return mkBuiltinTypeDiscriptor(builtintype_float);
        case BoolLiteral:
            return mkBuiltinTypeDiscriptor(builtintype_bool);
        case MessageLiteral:
        	return mkBuiltinTypeDiscriptor(builtintype_msg);
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
	case builtintype_int8:  /* Fall through */
	case builtintype_int16: /* Fall through */
	case builtintype_int32: /* Fall through */
	case builtintype_int64: /* Fall through */
	case builtintype_int:   /* Fall through */
	case builtintype_unknownInt:
        return 1;
    default:
        return 0;
    }
}

int typeMatch(Type *a, Type *b){
	if(a == NULL || b == NULL)
		return 0;

	if(a->tag != b->tag)
		return 0;

	if (a->tag == BuiltinTypeTag)
		return buildinTypeMatch(b, a->tags.typeBuiltin.builtinType);

	if(a->tag == StructTypeTag){
		if(strcmp(a->tags.typeStruct.name, b->tags.typeStruct.name) == 0)
			return 1;
		else
			return 0;
	}
	/*mangler at sammenligne alle andre typer end builtin typer hehexd*/
	return 0;
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

	return 0;
}

void printTypeFail(char *fail_message, AstNode *node, Type *type){
    char *text_type = typeString(type);
    errors++;
    printf("%s:%d: %s. Actual type: %s \n", filename, node->linenum, fail_message, text_type);
    free(text_type);
}

char *typeString(Type *type){
	char *elementtype, *result;

    if(type == NULL)
        return smprintf("undefined");

    switch(type->tag){
    case ArrayTypeTag:
    	elementtype = typeString(type->tags.typeArray.elementType);
    	if(type->tags.typeArray.size == -1)
    		result = smprintf("%s[]", elementtype);
    	else
    		result = smprintf("%s[%d]", elementtype, type->tags.typeArray.size);
    	free(elementtype);
        return result;
	case FunctionTypeTag:
        return smprintf("Function Type");
	case TaskTypeTag:
        return smprintf("Task Type");
	case BuiltinTypeTag:
        return smprintf("%s", builtintypeNames[type->tags.typeBuiltin.builtinType]);
	case StructTypeTag:
        return smprintf("struct %s", type->tags.typeStruct.name);
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


void checkFunctionCall(AstNode *node){	
	AstNode *id = node->node.FunctionCall.identifier;
	Type *t = id->node.Identifier.symbol->type;
	int parametercount;
	Type **parameters;
	AstNode *arg = node->node.FunctionCall.arguments;
	int paramnr = 0;

	if (t->tag != FunctionTypeTag){
		errors++;
		printf("%s:%d: %s is not a function\n", filename, id->linenum, id->node.Identifier.identifier);
		return;
	}
	parametercount = t->tags.typeFunction.arity;
	parameters = t->tags.typeFunction.parameterTypes;

	for(; paramnr < parametercount && arg != NULL; paramnr++ , arg = arg->next){
		Type *ptype = parameters[paramnr];
		Type *atype = typeof(arg);
		if(!typeMatch(ptype, atype)){
			char *expected = typeString(ptype);	
			char *errorMsg = smprintf("Expected argument %d of function \"%s\" to have type %s", paramnr + 1, id->node.Identifier.identifier, expected);
			printTypeFail(errorMsg, arg, atype);
			free(expected);
			free(errorMsg);
		}
	}
	if(paramnr != parametercount|| arg != NULL){
		errors++;
		printf("%s:%d: Number of arguments in functioncall \"%s\" does not match the prototype. Expected %d, but got %d\n", filename, id->linenum, id->node.Identifier.identifier, parametercount, nodeLength(node->node.FunctionCall.arguments));
	}
}

/*checkSpawnNode is copy pasted from checkFunctionCall because they are syntactically similar*/
void checkSpawnNode(AstNode *node){	
	AstNode *id = node->node.Spawn.identifier;
	Type *t = id->node.Identifier.symbol->type;
	int parametercount;
	Type **parameters;
	AstNode *arg = node->node.Spawn.arguments;
	int paramnr = 0;

	if (t->tag != TaskTypeTag){
		errors++;
		printf("%s:%d: %s is not a task\n", filename, id->linenum, id->node.Identifier.identifier);
		return;
	}
	parametercount = t->tags.typeTask.arity;
	parameters = t->tags.typeTask.parameterTypes;

	for(; paramnr < parametercount && arg != NULL; paramnr++ , arg = arg->next){
		Type *ptype = parameters[paramnr];
		Type *atype = typeof(arg);
		if(!typeMatch(ptype, atype)){
			char *expected = typeString(ptype);	
			char *errorMsg = smprintf("Expected argument %d of task \"%s\" to have type %s", paramnr + 1, id->node.Identifier.identifier, expected);
			printTypeFail(errorMsg, arg, atype);
			free(expected);
			free(errorMsg);
		}
	}
	if(paramnr != parametercount|| arg != NULL){
		errors++;
		printf("%s:%d: Number of arguments in taskspawn \"%s\" is incorrect. Expected %d, but got %d\n", filename, id->linenum, id->node.Identifier.identifier, parametercount, nodeLength(node->node.Spawn.arguments));
	}
}