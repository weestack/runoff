#include <stdio.h>
#include "data.h"

static void insertInitCode(AstNode *);
static AstNode *arrayIndexNode(Type *, int);

void initializeVars(AstNode *tree){
	if(tree == NULL)
		return;

	if(tree->tag == ExprStmt && tree->node.ExprStmt.expression->tag == VarDecl)
		insertInitCode(tree);
	else
		initializeVars(tree->children);

	if(tree->chain != NULL)
		initializeVars(tree->chain);
}

static void insertInitCode(AstNode *exprstmt){
	AstNode *decl = exprstmt->node.ExprStmt.expression;
	Symbol *sym = decl->node.VarDecl.identifier->node.Identifier.symbol;
	Type *type = sym->type;

	if(type->tag == BuiltinTypeTag && !sym->initializedVar)
		decl->node.VarDecl.expression = getDefaultValue(type);

	if(type->tag == ArrayTypeTag && getDefaultValue(arrayBaseType(type)) != NULL){
		int i;
		AstNode *list = exprstmt;
		for(i = 0; i < fullArraySize(type); i++){
			AstNode *index, *loc, *expr, *assign;
			
			if(sym->initializedArray[i] == 1)
				continue;

			index = arrayIndexNode(type, i);
			loc = mkArrayLocationNode(decl->node.VarDecl.identifier, index);
			expr = getDefaultValue(arrayBaseType(type));
			assign = mkExprStmtNode(mkAssignmentNode(loc, expr));
			assign->next = list->next;
			assign->chain = list->chain;
			list->next = assign;
			list->chain = assign;
			list = list->next;
		}
	}
}

static AstNode *arrayIndexNode(Type *t, int index){
	AstNode *result = NULL;
	while(t->tag == ArrayTypeTag){
		AstNode *new = result;
		int i = index % t->tags.typeArray.size;
		index = index / t->tags.typeArray.size;
		new = mkIntLiteralNode(i);
		result = append_node(result, new);

		t = t->tags.typeArray.elementType;
	}
	return result;
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