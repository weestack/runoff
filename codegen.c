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

char *codegen(AstNode *tree) { 
	char *id = NULL;
	char *type = NULL;
	char *params = NULL;
	char *stmts = NULL;
	char *intlit = NULL;

	char *result = NULL;

	if (tree == NULL) return smprintf("");

	switch (tree->tag) {
		case Prog:
			result = processBlock(tree->node.Prog.toplevels, "\n", 0);
			break;
		case DefineFunction:
			type = codegen(tree->node.DefineFunction.type);
			id = codegen(tree->node.DefineFunction.identifier);
			params = processBlock(tree->node.DefineFunction.parameters, ", ", 0);
			stmts = processBlock(tree->node.DefineFunction.statements, ";", 1);
			result = smprintf("%s %s(%s) {%s}", type, id, params, stmts);
			break;
		case Parameter:
			type = codegen(tree->node.Parameter.type);
			id = codegen(tree->node.Parameter.identifier);
			intlit = tree->node.Parameter.type->tag == ArrayType ? smprintf("[%s]", tree->node.Parameter.type->node.ArrayType.int_literal) : smprintf("");
			result = smprintf("%s %s%s", type, id, intlit);
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
		case Identifier:
			result = smprintf("%s", tree->node.Identifier.symbol->name);
			break;
		default:
			result = smprintf("");
	}

	free(id);
	free(type);
	free(params);
	free(stmts);
	free(intlit);

	return result;
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
			return "unsigned long";
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
			return "long";
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
			return "boolean";
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