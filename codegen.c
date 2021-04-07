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

char *generateNodeCode(char *code, AstNode *tree);
void processBlock(char *code, AstNode *tree);
char *getBuiltInTypeLiteral(int type);

char *codegen(AstNode *tree) {
	/* AstNode *children = getChildren(tree); */
	char *code = malloc(1000000);
	generateNodeCode(code, tree);
	return code;
}

void processBlock(char* code, AstNode *tree) {
	AstNode *child;
	for (child = tree; child != NULL; child = child->next) {
		strcat(code, generateNodeCode(code, child));
	}
}

char *generateNodeCode(char *code, AstNode *tree) {

	switch (tree->tag) {
		case Prog:
			processBlock(code, tree->node.Prog.toplevels);
			break;
		case DefineFunction:
			strcat(code, getBuiltInTypeLiteral(tree->node.DefineFunction.type->node.BuiltinType.type));
			strcat(code, smprintf(" %s(", tree->node.DefineFunction.identifier->node.Identifier.symbol->name));
			processBlock(code, tree->node.DefineFunction.parameters);
			strcat(code, ") {");
			
			processBlock(code, tree->node.DefineFunction.statements);

			strcat(code, "}");
			

			break;
		case Parameter:
			/*
			strcat(code, smprintf("%s ", getBuiltInTypeLiteral(tree->node.Parameter.type->node.BuiltinType.type)));
			strcat(code, tree->node.Parameter.identifier->node.Identifier.symbol->name);
			if (tree->node.next != NULL) strcat(code, ", ");
			*/
			break;
		default:
			return "";
	}

	return code;
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