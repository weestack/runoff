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

	char *result = NULL;

	if (tree == NULL) return smprintf("");

	switch (tree->tag) {
		case Prog:
			result = processBlock(tree->node.Prog.toplevels, "\n", 0);
			break;
		case DefineFunction:
			type = smprintf("%s", getBuiltInTypeLiteral(tree->node.DefineFunction.type->node.BuiltinType.type));
			id = smprintf("%s", tree->node.DefineFunction.identifier->node.Identifier.symbol->name);
			params = processBlock(tree->node.DefineFunction.parameters, ", ", 0);
			stmts = processBlock(tree->node.DefineFunction.statements, ";", 1);
			result = smprintf("%s %s(%s) {%s}", type, id, params, stmts);
			break;
		case Parameter:
			type = smprintf("%s", getBuiltInTypeLiteral(tree->node.Parameter.type->node.BuiltinType.type));
			id = smprintf("%s", tree->node.Parameter.identifier->node.Identifier.symbol->name);
			result = smprintf("%s %s", type, id);
			break;
		default:
			result = smprintf("");
	}

	free(id);
	free(type);
	free(params);
	free(stmts);

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