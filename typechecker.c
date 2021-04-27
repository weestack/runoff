#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "auxiliary.h"

void typeCheckNode(AstNode *);
int buildinTypeMatch(Type *, int);
int buildinTypeMatchInt(Type *);
int numericType(Type *);
void printTypeFail(char *, AstNode *, Type *);
char *typeString(Type *);
Type *binaryOperatorType(AstNode *);
Type *unaryOperatorType(AstNode *);
int typeMatch(Type *, Type *);
void checkParameterList(AstNode *id, AstNode *args, int arity, Type **parameters, int typetag, char *desc);

static int errors;

int typeCheck(AstNode *tree){
	typeCheckNode(tree);
	return errors;
}

void typeCheckNode(AstNode *node){
	Type *typeA, *typeB, *typeC;
	AstNode *children;
	AstNode *id;

	switch(node->tag){
	case While:
		typeA = typeOf(node->node.While.expression);
		if(!buildinTypeMatch(typeA, BuiltinTypeBool))
			printTypeFail("While loop expected bool", node->node.While.expression, typeA);

		break;
	case For:
		if(node->node.For.expressionTest == NULL){
			break;
		}
		typeA = typeOf(node->node.For.expressionTest);
		if(!buildinTypeMatch(typeA, BuiltinTypeBool))
			printTypeFail("For loop test expected bool", node->node.For.expressionTest, typeA);

		break;
	case Switch:
		typeA = typeOf(node->node.Switch.expression);
		if(!buildinTypeMatchInt(typeA))
			printTypeFail("Switch expression expected int", node->node.Switch.expression, typeA);

		break;
	case If:
		typeA = typeOf(node->node.If.expression);
		if(!buildinTypeMatch(typeA, BuiltinTypeBool))
			printTypeFail("if statement expected bool", node->node.If.expression, typeA);

		break;
	case ElseIf:
		typeA = typeOf(node->node.ElseIf.expression);
		if(!buildinTypeMatch(typeA, BuiltinTypeBool))
			printTypeFail("if statement expected bool", node->node.ElseIf.expression, typeA);
		break;
	case VarDecl:
		if(node->node.VarDecl.expression == NULL)
			break;

		typeA = typeOf(node->node.VarDecl.identifier);
		typeB = typeOf(node->node.VarDecl.expression);
		if (!typeMatch(typeA, typeB)){
			char* typeAString = typeString(typeA);
			char* errorMessage = smprintf("incompatible types expected %s", typeAString);
			printTypeFail(errorMessage, node->node.VarDecl.expression, typeB);
			free(typeAString);
			free(errorMessage);
		}
		break;
	case FunctionCall:
		id = node->node.FunctionCall.identifier;
		typeA = id->node.Identifier.symbol->type;
		checkParameterList(id,
			node->node.FunctionCall.arguments,
			typeA->tags.typeFunction.arity,
			typeA->tags.typeFunction.parameterTypes,
			FunctionTypeTag,
			"function-call");
		break;
	case Assignment:
		typeA = typeOf(node->node.Assignment.location);
		typeB = typeOf(node->node.Assignment.expression);
		if(!typeMatch(typeA, typeB)){
			char* typeAString = typeString(typeA);
			char* errorMessage = smprintf("incompatible types expected %s", typeAString);
			printTypeFail(errorMessage, node->node.Assignment.location, typeB);
			free(typeAString);
			free(errorMessage);
		}
		break;
	case TernaryOperator:
		typeA = typeOf(node->node.TernaryOperator.expressionTest);
		typeB = typeOf(node->node.TernaryOperator.expressionTrue);
		typeC = typeOf(node->node.TernaryOperator.expressionFalse);
		if(!typeMatch(typeB, typeC)){
			char* typeBString = typeString(typeB);
			char* errorMessage = smprintf("incompatible types expected %s", typeBString);
			printTypeFail(errorMessage, node->node.TernaryOperator.expressionTrue, typeC);
			free(typeBString);
			free(errorMessage);
		}
		if(!buildinTypeMatch(typeA, BuiltinTypeBool)){
			printTypeFail("ternary operator test must be boolean", node->node.TernaryOperator.expressionTest, typeA);
		}
		break;
	case MessageLiteral:
		id = node->node.MessageLiteral.identifier;
		typeA = id->node.Identifier.symbol->type;
		checkParameterList(id,
			node->node.MessageLiteral.arguments,
			typeA->tags.typeMessage.arity,
			typeA->tags.typeMessage.parameterTypes,
			MessageTypeTag,
			"message literal");
		break;
	case Return:
		typeA = typeOf(node->node.Return.expression);
		
		if(node->node.Return.functionsym == NULL){
			if(typeA != NULL){
				errors++;
				eprintf(node->linenum, "Unexpected expession in return of task\n");
				break;
			}
			break;
		}

		typeB = node->node.Return.functionsym->type->tags.typeFunction.returnType;

		if(typeA == NULL && !buildinTypeMatch(typeB, BuiltinTypeVoid)){
			char *expected = typeString(typeB);
			errors++;
			eprintf(node->linenum, "Missing expression of type %s in return statement\n", expected);
			free(expected);
			break;
		}
		if(typeA != NULL && !typeMatch(typeA, typeB)){
			char* typeBString = typeString(typeB);
			char* errorMessage = smprintf("incompatible types in return. Expected: %s", typeBString);
			printTypeFail(errorMessage, node->node.Return.expression, typeA);
			free(typeBString);
			free(errorMessage);
		}
		break;
	case Spawn:
		id = node->node.Spawn.identifier;
		typeA = id->node.Identifier.symbol->type;
		checkParameterList(id,
			node->node.Spawn.arguments,
			typeA->tags.typeTask.arity,
			typeA->tags.typeTask.parameterTypes,
			TaskTypeTag,
			"task");
		break;
	case Send:
		typeA = typeOf(node->node.Send.message);
		typeB = typeOf(node->node.Send.receiver);
		if(!buildinTypeMatch(typeA, BuiltinTypeMsg))
			printTypeFail("body of send should be a message", node->node.Send.message, typeA);
		if(!buildinTypeMatch(typeB, BuiltinTypeTaskid))
			printTypeFail("receiver of send should be a taskid", node->node.Send.receiver, typeB);
		break;
	case BinaryOperation: /* Fall through */
	case UnaryOperation: /* Fall through */
	case ExprStmt:
		typeOf(node); /* typeOf will check that the expression is ok (it tries to find its type) */
		break;
	case ReceiveCase:
		typeA = typeOf(node->node.ReceiveCase.messageName);
		if(typeA == NULL) /* default case */
			break;
		if(!(typeA->tag == MessageTypeTag))
			printTypeFail("Name in receive case should be a message type.", node, typeA);
		break;
	}

	children = node->children;
	for(; children!=NULL; children=children->chain)
		typeCheckNode(children);
}

Type *typeOf(AstNode *node){
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
					eprintf(id->linenum, "Index in array %s too deep\n", id->node.Identifier.identifier);
					return NULL;
				}
			}
			return tmp;
		case StructLocation:
			return typeOf(node->node.StructLocation.location);
		case VariableLocation:
			id = node->node.VariableLocation.identifier;
			return id->node.Identifier.symbol->type;
		case Identifier:
			return node->node.Identifier.symbol->type;
		case PinLiteral:
			return mkBuiltinTypeDescriptor(BuiltinTypePinid);
		case IntLiteral:
			return mkBuiltinTypeDescriptor(BuiltinTypeUnknownInt);
		case FloatLiteral:
			return mkBuiltinTypeDescriptor(BuiltinTypeFloat);
		case BoolLiteral:
			return mkBuiltinTypeDescriptor(BuiltinTypeBool);
		case MessageLiteral:
			return mkBuiltinTypeDescriptor(BuiltinTypeMsg);
		case BinaryOperation:
			return binaryOperatorType(node);
		case UnaryOperation:
			return unaryOperatorType(node);
		case FunctionCall:
			id = node->node.FunctionCall.identifier;
			return id->node.Identifier.symbol->type->tags.typeFunction.returnType;
		case Assignment:
			return typeOf(node->node.Assignment.location);
		case VarDecl:
			id = node->node.VarDecl.identifier;
			return id->node.Identifier.symbol->type;
		case TernaryOperator:
			return typeOf(node->node.TernaryOperator.expressionTrue);
		case Spawn:
			return mkBuiltinTypeDescriptor(BuiltinTypeTaskid);
		default:
			return NULL;
	}
	return NULL;
}

int buildinTypeMatch(Type *a, int b){
	Type *bType = mkBuiltinTypeDescriptor(b);
	if(a == NULL)
		return 0;

	if(a->tag == BuiltinTypeTag){
		if (a->tags.typeBuiltin.builtinType == b){
			return 1;
		}
		else if(a->tags.typeBuiltin.builtinType == BuiltinTypeUnknownInt && buildinTypeMatchInt(bType)){
			return 1;
		}
		else if (b == BuiltinTypeUnknownInt && buildinTypeMatchInt(a)){
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
	case BuiltinTypeUint8: /* Fall through */
	case BuiltinTypeUint16: /* Fall through */
	case BuiltinTypeUint32: /* Fall through */
	case BuiltinTypeUint64: /* Fall through */
	case BuiltinTypeInt8:  /* Fall through */
	case BuiltinTypeInt16: /* Fall through */
	case BuiltinTypeInt32: /* Fall through */
	case BuiltinTypeInt64: /* Fall through */
	case BuiltinTypeUnknownInt:
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

	if(a->tag == ArrayTypeTag){
		if (typeMatch(a->tags.typeArray.elementType, b->tags.typeArray.elementType)) {
			if (a->tags.typeArray.size == b->tags.typeArray.size) {
				return 1;
			}
		}
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

	if(type->tags.typeBuiltin.builtinType == BuiltinTypeFloat)
		return 1;

	return 0;
}

void printTypeFail(char *fail_message, AstNode *node, Type *type){
	char *text_type = typeString(type);
	errors++;
	eprintf(node->linenum, "%s. Actual type: %s \n", fail_message, text_type);
	free(text_type);
}

char *typeString(Type *type){
	char *elementtype, *result;
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
	Type *left = typeOf(node->node.BinaryOperation.expression_left);
	Type *right = typeOf(node->node.BinaryOperation.expression_right);
	int expected_right = 0, expected_left = 0, return_type = 0;
	char *other_error = NULL;

	switch(node->node.BinaryOperation.operator){
	case OpLogAnd: /*Fall through*/
	case OpLogOr:
		expected_left = expected_right = return_type = BuiltinTypeBool;
		break;
	case OpSmallerEqual: /*Fall through*/
	case OpGreaterEqual: /*Fall through*/
	case OpSmallerThan:  /*Fall through*/
	case OpGreaterThan:
		return_type = BuiltinTypeBool;
		if(!numericType(left)){
			other_error = "Expected numeric types";
			break;
		}
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
		break;
	case OpEqual: /*Fall through*/
	case OpNotEqual:
		return_type = BuiltinTypeBool;
		if(left == NULL || left->tag != BuiltinTypeTag){
			other_error = "Expected builtin types";
			break;
		}
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
		break;
	case OpPlus: /*Fall through*/
	case OpMinus:/*Fall through*/
	case OpTimes:/*Fall through*/
	case OpDivide:
		if(!numericType(left)){
			other_error = "Expected numeric types";
			break;
		}
		expected_left = left->tags.typeBuiltin.builtinType;
		expected_right = expected_left;
		return_type = expected_right;
		break;
	case OpMod:		 /*Fall through*/
	case OpBitAnd:	 /*Fall through*/
	case OpBitOr:	  /*Fall through*/
	case OpShiftRight: /*Fall through*/
	case OpShiftLeft:  /*Fall through*/
	case OpBitXor:
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
		eprintf(node->linenum, "binary operator %s %s.\n\tActual type for left: %s\n\tActual type for right: %s \n",
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
	return mkBuiltinTypeDescriptor(return_type);

}

Type *unaryOperatorType(AstNode *node){
	Type *operandType = typeOf(node->node.UnaryOperation.expression);
	char *otherError = NULL;
	int expected = 0, returnType = 0;
	switch (node->node.UnaryOperation.operator){
	case OpLogNot:
		expected = BuiltinTypeBool;
		returnType = expected;
		break;
	case OpDecrement: /*Fall through*/
	case OpIncrement: /*Fall through*/
	case OpBitNot:
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
	return mkBuiltinTypeDescriptor(returnType);
}

void checkParameterList(AstNode *id, AstNode *args, int arity, Type **parameters, int typetag, char *desc){
	Type *t = id->node.Identifier.symbol->type;
	AstNode *arg = args;
	int paramnr;
	char *name = id->node.Identifier.identifier;
	
	if(t->tag != typetag){
		errors++;
		eprintf(id->linenum, "%s is not a %s\n", name, desc);
		return;
	}
	
	for(paramnr = 0; paramnr < arity && arg != NULL; paramnr++, arg = arg->next){
		Type *ptype = parameters[paramnr];
		Type *atype = typeOf(arg);
		if(!typeMatch(ptype, atype)){
			char *expected = typeString(ptype);
			char *errmsg = smprintf("Expected argument %d of %s \"%s\" to have type %s", paramnr+1, desc, name, expected);
			printTypeFail(errmsg, arg, atype);
			free(expected);
			free(errmsg);
		}
	}
	if(paramnr != arity || arg != NULL){
		errors++;
		eprintf(id->linenum, "Number of argument in %s \"%s\" incorrect. Expected %d, got %d\n", desc, name, arity, nodeLength(args));
	}
}