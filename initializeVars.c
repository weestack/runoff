#include <stdio.h>
#include "data.h"

static void insertInitCode(AstNode *);

void initializeVars(AstNode *tree){
	if(tree == NULL)
		return;

	if(tree->tag == VarDecl){
		insertInitCode(tree);
	}else{
		initializeVars(tree->children);
	}

	if(tree->chain != NULL)
		initializeVars(tree->chain);
}

static void insertInitCode(AstNode *decl){
	Symbol *sym = decl->node.VarDecl.identifier->node.Identifier.symbol;
	Type *type = sym->type;

	if(type->tag == BuiltinTypeTag && !sym->initializedVar)
		decl->node.VarDecl.expression = getDefaultValue(type);
}

AstNode *getDefaultValue(Type *type){
	if(type->tag != BuiltinTypeTag)
		return NULL;

	switch(type->tags.typeBuiltin.builtinType){
	case BuiltinTypeUint8:
	case BuiltinTypeUint16:
	case BuiltinTypeUint32:
	case BuiltinTypeUint64:
	case BuiltinTypeInt8:
	case BuiltinTypeInt16:
	case BuiltinTypeInt32:
	case BuiltinTypeInt64:
		return mkIntLiteralNode(0);
	case BuiltinTypeFloat:
		return mkFloatLiteralNode(0);
	case BuiltinTypeBool:
		return mkBoolLiteralNode(0);
	default:
		return NULL;
	}
}