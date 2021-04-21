#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"


/* Tænker at hvis codegen returnere en stor streng fra heapen
	lidt i stil med prettyprint, så er det smart, for så kan vi
	selv bestemme om main() i runoff.c skal printe eller putte
	over i en fil eller hvad vi nu vil gøre med strengen. */

char *processBlock(AstNode *node, char *sep, int end);
char *getBuiltInTypeLiteral(int type);
char *getBinaryOperator(int operator);
char *getHelperFunctionsCode(void);
char *buildArrayDeclIndices(AstNode *node);
char *generatePassByValue(AstNode *tree);
char *constructMessageEnum(AstNode *tree);
char *constructMessageStruct(AstNode *tree);
char *constructMessageUnionStruct(AstNode *tree);

char *codegen(AstNode *tree) {
	char *id = NULL;
	char *type = NULL;
	char *params = NULL;
	char *stmts = NULL;
	char *intlit = NULL;
	char *expr = NULL;
	char *elsepart = NULL;
	char *location = NULL;
	char *indicies = NULL;
	char *extraCode = NULL;
	char *preCodeGen = NULL;

	char *result = NULL;

	if (tree == NULL) return smprintf("");

	switch (tree->tag) {
		case Prog:
			/* Special case include helper functions! */
			preCodeGen = smprintf("%s\n#define numberOfSpawns %d\ntaskHandle_t handlers[numberOfSpawns];\n/*End of preCodeGen*/\n", getHelperFunctionsCode(), tree->node.Prog.spawnCount);
			result = smprintf("%s\n%s", preCodeGen, processBlock(tree->node.Prog.toplevels, "\n", 0));
			break;
		case DefineFunction:
			type = codegen(tree->node.DefineFunction.type);
			id = codegen(tree->node.DefineFunction.identifier);
			params = processBlock(tree->node.DefineFunction.parameters, ", ", 0);
			extraCode = generatePassByValue( tree->node.DefineFunction.parameters );
			stmts = processBlock(tree->node.DefineFunction.statements, "\n", 1);
			result = smprintf("%s %s(%s) {%s %s}", type, id, params, extraCode, stmts);
			break;
		case DefineTask:
			id = codegen(tree->node.DefineTask.identifier);
			params = processBlock(tree->node.DefineTask.parameters, ", ", 0);
			extraCode = generatePassByValue( tree->node.DefineTask.parameters );
			stmts = processBlock(tree->node.DefineTask.statements, "\n", 1);
			result = smprintf("void %s(%s) {%s %s}", id, params, extraCode, stmts);
			break;
		case DefineStruct:
			result = smprintf(
				"struct %s {%s};",
				codegen(tree->node.DefineStruct.identifier),
				processBlock(tree->node.DefineStruct.fields, "", 0)
			);
			break;
		case DefineMessage:
			result = smprintf("enum messages{%s};\n%s\n%s",
							constructMessageEnum(tree->node.DefineMessage.messagesIdentifiers),
							constructMessageStruct(tree->node.DefineMessage.messagesIdentifiers),
							constructMessageUnionStruct(tree->node.DefineMessage.messagesIdentifiers)
			);
			break;
		case StructMember:
			result = smprintf(
				"%s %s;",
				codegen(tree->node.StructMember.type),
				codegen(tree->node.StructMember.identifier)
			);
			break;
		case Parameter:
			type = codegen(tree->node.Parameter.type);

			if (tree->node.Parameter.type->tag == ArrayType) {
				id = smprintf("%s_original",codegen(tree->node.Parameter.identifier));
			}else {
				id = codegen(tree->node.Parameter.identifier);
			}
			intlit = tree->node.Parameter.type->tag == ArrayType ? buildArrayDeclIndices(tree->node.Parameter.type) : smprintf("");
			result = smprintf("%s %s%s", type, id, intlit);
			break;
		case ElseIf:
			expr = codegen(tree->node.ElseIf.expression);
			stmts = processBlock(tree->node.ElseIf.statements, "", 1);
			elsepart = codegen(tree->node.ElseIf.elsePart);
			result = smprintf("else if (%s) {%s} %s", expr, stmts, elsepart);
			break;
		case If:
			expr = codegen(tree->node.If.expression);
			stmts = processBlock(tree->node.If.statements, "", 1);
			elsepart = codegen(tree->node.If.elsePart);
			result = smprintf("if (%s) {%s} %s", expr, stmts, elsepart);
			break;
		case Else:
			stmts = processBlock(tree->node.Else.statements, "", 1);
			result = smprintf("else {%s}", stmts);
			break;
		case Receive:
			stmts = processBlock(tree->node.Receive.cases, "", 1);
			result = smprintf("receive{%s}", stmts);
			break;
		/*case ReceiveCase:

			break;*/
		case Spawn:
			if(tree->node.Spawn.arguments != NULL){
				result = smprintf("runoff_createTask(%s, %s)", 
					codegen(tree->node.Spawn.identifier),
					codegen(tree->node.Spawn.arguments)
				);
			} else {
				result = smprintf("runoff_createTask(%s)", 
					codegen(tree->node.Spawn.identifier));
			}
			break;
		case BuiltinType:
			result = smprintf("%s", getBuiltInTypeLiteral(tree->node.BuiltinType.type));
			break;
		case StructType:
			id = codegen(tree->node.StructType.identifier);
			result = smprintf("struct %s", id);
			break;
		case ArrayType:
			type = codegen(tree->node.ArrayType.type);
			result = smprintf("%s", type);
			break;
		case While:
			expr = codegen(tree->node.While.expression);
			stmts = processBlock(tree->node.While.statements, "", 0);
			result = smprintf("while(%s) {%s}", expr, stmts);
		 	break;
		case For:
			result = smprintf(
				"for(%s;%s;%s){%s}",
				codegen(tree->node.For.expressionInit),
				codegen(tree->node.For.expressionTest),
				codegen(tree->node.For.expressionUpdate),
				processBlock(tree->node.For.statements, "", 0)
			);
			break;
		case Switch:
			expr = codegen(tree->node.Switch.expression);
			result = smprintf("switch(%s){%s}",
				expr,
				processBlock(tree->node.Switch.cases, "", 0)
			);
			break;
		case SwitchCase:
			stmts = processBlock(tree->node.SwitchCase.statements, "", 0);
			if (tree->node.SwitchCase.literal != NULL) {
				result = smprintf("case %s: %s break;",
					codegen(tree->node.SwitchCase.literal),
					stmts
				);
			}else {
				result = smprintf("default: %s break;", stmts);
			}
		break;
		case Identifier:
			result = smprintf("%s", tree->node.Identifier.symbol->name);
			break;
		case ExprStmt:
			result = smprintf("%s;", codegen(tree->node.ExprStmt.expression));
			break;
		case Assignment:
			id = codegen(tree->node.Assignment.location);
			expr = processBlock(tree->node.Assignment.expression, "", 0);
			result = smprintf("(%s = %s)", id, expr);
			break;
		case TernaryOperator:
			result = smprintf("(%s) ? %s: %s",
			codegen(tree->node.TernaryOperator.expressionTest),
			codegen(tree->node.TernaryOperator.expressionTrue),
			codegen(tree->node.TernaryOperator.expressionFalse)
		);
			break;
		case VariableLocation:
			result = codegen(tree->node.VariableLocation.identifier);
			break;
		case StructLocation:
			id = codegen(tree->node.StructLocation.identifier);
			location = codegen(tree->node.StructLocation.location);
			result = smprintf("%s.%s", id, location);
			break;
		case ArrayLocation:
			id = codegen(tree->node.ArrayLocation.identifier);
			indicies = processBlock(tree->node.ArrayLocation.indicies, "][", 0);
			result = smprintf("%s[%s]", id, indicies);
			break;
		case UnaryOperation:
			expr = codegen(tree->node.UnaryOperation.expression);
			if (tree->node.UnaryOperation.fix == postfix) {
				result= smprintf("%s%s", expr, getBinaryOperator(tree->node.UnaryOperation.operator));
			}else {
				result= smprintf("%s%s", getBinaryOperator(tree->node.UnaryOperation.operator), expr);
			}
			break;
		case Return:
			expr = codegen(tree->node.Return.expression);
			result = smprintf("return %s;", expr);
			break;
		case FunctionCall:
			id = codegen(tree->node.FunctionCall.identifier);
			params = processBlock(tree->node.FunctionCall.arguments, ",", 0);
			result = smprintf("%s(%s)", id, params);
			break;
		case VarDecl:
			type = smprintf("%s%s", tree->node.VarDecl.toplevel == 1 ? "const " : "", codegen(tree->node.VarDecl.type));
			id = codegen(tree->node.VarDecl.identifier);
			intlit = tree->node.VarDecl.type->tag == ArrayType ? buildArrayDeclIndices(tree->node.VarDecl.type) : smprintf("");
			expr = tree->node.VarDecl.expression != NULL ? smprintf(" = %s", codegen(tree->node.VarDecl.expression)) : smprintf("");
			result = smprintf("%s %s%s%s%s", type, id, intlit, expr, tree->node.VarDecl.toplevel == 1 ? ";" : "");
			break;
		case BinaryOperation:
			result = smprintf("(%s %s %s)",
			codegen(tree->node.BinaryOperation.expression_left),
			getBinaryOperator(tree->node.BinaryOperation.operator),
			codegen(tree->node.BinaryOperation.expression_right));
			break;
		case IntLiteral:
			result = smprintf("%d", tree->node.IntLiteral.value);
			break;
		case PinLiteral:
			result = smprintf("%d", tree->node.PinLiteral.value);
			break;
		case FloatLiteral:
			result = smprintf("%lf", tree->node.FloatLiteral.value);
			break;
		case BoolLiteral:
			result = smprintf("%s", tree->node.BoolLiteral.value ? "true" : "false");
			break;
		default:
			result = smprintf("");
	}

	free(id);
	free(type);
	free(params);
	free(stmts);
	free(intlit);
	free(expr);
	free(elsepart);
	free(location);
	free(indicies);
	free(extraCode);
	free(preCodeGen);

	return result;
}

char *getBinaryOperator(int operator) {
	switch (operator) {
		case elogical_and:
			return "&&";
			break;
		case elogical_or:
			return "||";
			break;
		case egreater_equal:
			return ">=";
			break;
		case esmaller_equal:
			return "<=";
			break;
		case eequal:
			return "==";
			break;
		case enot_equal:
			return "!=";
			break;
		case edecrement:
			return "--";
			break;
		case eincrement:
			return "++";
			break;
		case esmaller_than:
			return "<";
			break;
		case ebigger_than:
			return ">";
			break;
		case emod:
			return "%";
			break;
		case eplus:
			return "+";
			break;
		case eminus:
			return "-";
			break;
		case etimes:
			return "*";
			break;
		case edivid:
			return "/";
			break;
		case elogical_not:
			return "!";
			break;
		case ebit_and:
			return "&";
			break;
		case ebit_or:
			return "|";
			break;
		case eright_shift:
			return ">";
			break;
		case eleft_shift:
			return "<";
			break;
		case ebit_xor:
			return "^";
			break;
		case ebit_not:
			return "~";
			break;
		default:
			return "unknown_op";
	}
}


char *buildArrayDeclIndices(AstNode *node) {
	char *buffer = smprintf("[%s]", codegen(node->node.ArrayType.int_literal));
	while (node->node.ArrayType.type->tag == ArrayType) {
		char *tmp = buffer;
		node = node->node.ArrayType.type;
		buffer = smprintf("[%s]%s", codegen(node->node.ArrayType.int_literal), buffer);
		free(tmp);
	}
	return buffer;
}
/* prettyprinterlist copy :joyd: */
char *processBlock(AstNode *node, char *sep, int end){
	char *result;
	char *prev;
	char *childstr;
	AstNode *child;

	if(node == NULL)
		return smprintf("");

	child = node->next;
	result = codegen(node);

	while(child != NULL){
		prev = result;
		childstr = codegen(child);
		result = smprintf("%s%s%s", prev, sep, childstr);
		free(prev);
		free(childstr);
		child = child->next;
	}
	if(end){
		prev = result;
		result = smprintf("%s%s", prev, sep);
		free(prev);
	}
	return result;
}

char *getBuiltInTypeLiteral(int type) {
	switch (type) {
		case builtintype_uint8:
			return "byte";
			break;
		case builtintype_uint16:
			return "unsigned int";
			break;
		case builtintype_uint32:
			return "unsigned long";
			break;
		case builtintype_uint64:
			return "uint64_t";
			break;
		case builtintype_int8:
			return "char";
			break;
		case builtintype_int16:
			return "int";
			break;
		case builtintype_int32:
			return "long";
			break;
		case builtintype_int64:
			return "int64_t";
			break;
		case builtintype_unknownInt:
			return "int";
			break;
		case builtintype_float:
			return "float";
			break;
		case builtintype_void:
			return "void";
			break;
		case builtintype_bool:
			return "bool";
			break;
		case builtintype_msg:
		case builtintype_taskid:
		case builtintype_pinid:
			return "int";
			break;
		default:
			return "unknownType";
	}
}


char *getHelperFunctionsCode(void) {
	FILE *fp;
	long length = 0;
	char *code;

	/* opening file for reading */
	fp = fopen("arduino_helpers.ino" , "r");

	if(fp == NULL) {
			return NULL;
	}


	if (fp) {
	  fseek (fp, 0, SEEK_END);
	  length = ftell(fp);
	  fseek(fp, 0, SEEK_SET);
	  code = malloc(length);
	  if (code){
	    fread(code, 1, length, fp);
	  }
	  fclose (fp);
	}

	return code;
}

char *generatePassByValue(AstNode *tree) {
	char *result = NULL;
	char *prev;
	char *childstr;
	AstNode *child;

	if (tree == NULL) return smprintf("");

	child = tree;
	result = smprintf("");
	while(child != NULL){
		prev = smprintf("%s", result);
		switch (child->node.Parameter.type->tag) {
			case ArrayType:
				childstr = smprintf("%s %s%s; memcpy(%s, %s, sizeof(%s));",
						codegen(child->node.Parameter.type->node.ArrayType.type),
						codegen(child->node.Parameter.identifier),
						buildArrayDeclIndices(tree->node.Parameter.type),
						codegen(child->node.Parameter.identifier),
						smprintf("%s_original",codegen(child->node.Parameter.identifier)),
						smprintf("%s_original",codegen(child->node.Parameter.identifier))
				);
				break;
			default:
				childstr = smprintf("");
		}
		result = smprintf("%s%s", prev, childstr);
		free(prev);
		free(childstr);
		child = child->next;
	}

	return result;
}


char *constructMessageEnum(AstNode *tree){
	char *result = smprintf("");
	char *currentChild;
	char *old;
	AstNode *child = tree;
	int counter = 0;

	while(child != NULL){
		currentChild = codegen(child->node.MessageIdentifier.identifier);
		old = result;
		result = smprintf("%s%s\n%s",
				result, (counter++ != 0) ? "," : "", currentChild
				);
		free(old);
		free(currentChild);
		child = child->next;
	}
	return result;
}

char *constructMessageStruct(AstNode *tree){
	char *currentChildId;
	char *currentChildType;
	AstNode *child = tree;
	AstNode *paramChild;
	char *codeBlock = smprintf("");
	char *currentStruct;
	char *old;

	while(child != NULL){
		paramChild = child->node.MessageIdentifier.parameters;
		currentStruct = smprintf("");
		while(paramChild != NULL){
			currentChildType = codegen(paramChild->node.Parameter.type);
			currentChildId = smprintf("run_%s",codegen(paramChild->node.Parameter.identifier));
			old = currentStruct;
			currentStruct = smprintf("%s %s;\n%s",
					currentChildType, currentChildId, currentStruct
					);
			free(old);
			free(currentChildId);
			free(currentChildType);
			paramChild = paramChild->next;
		}
		old = codeBlock;
		codeBlock = smprintf("struct %s{%s};\n%s",
			codegen(child->node.MessageIdentifier.identifier),
			currentStruct,
			codeBlock
		);

		free(old);
		free(currentStruct);
		child = child->next;
	}
	return codeBlock;
}

char *constructMessageUnionStruct(AstNode *tree){
	char *result;
	char *structNames = smprintf("");
	char *old;
	AstNode *child = tree;

	while(child != NULL){
		old = structNames;
		structNames = smprintf("struct %s %s; %s",
				codegen(child->node.MessageIdentifier.identifier),
				codegen(child->node.MessageIdentifier.identifier),
				structNames);
		free(old);
		child = child->next;
	}
	result = smprintf("struct Message { int Tag; union {%s} data;};", structNames);

	free(structNames);
	return result;
}
