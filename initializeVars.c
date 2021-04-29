#include <stdio.h>
#include "data.h"

static void insertInitCode(AstNode *);

void initializeVars(AstNode *tree){
	if(tree == NULL)
		return;

	if(tree->tag == ExprStmt && tree->node.ExprStmt.expression->tag == VarDecl){
		printf("Before insertion %p\n", (void*) tree->chain);
		insertInitCode(tree);
		printf("After insertion %p\n", (void*) tree->chain);
	}else{
		initializeVars(tree->children);
	}

	if(tree->chain != NULL)
		initializeVars(tree->chain);
}

static void insertInitCode(AstNode *exprstmt){
	AstNode *decl = exprstmt->node.ExprStmt.expression;
	Symbol *sym = decl->node.VarDecl.identifier->node.Identifier.symbol;
	Type *type = sym->type;

	if(type->tag == BuiltinTypeTag && !sym->initializedVar)
		decl->node.VarDecl.expression = getDefaultValue(type);

	if(type->tag == ArrayTypeTag){
		int i;
		AstNode *list = exprstmt;
		printf("INIT ARRAY FIELDSS\n");
		for(i = 0; i < type->tags.typeArray.size; i++){
			AstNode *index, *loc, *expr, *assign;
			
			if(sym->initializedArray[i] == 1)
				continue;

			index = mkIntLiteralNode(i);
			loc = mkArrayLocationNode(decl->node.VarDecl.identifier, index);
			expr = getDefaultValue(type->tags.typeArray.elementType);
			assign = mkExprStmtNode(mkAssignmentNode(loc, expr));
			printf("Made assignment node %d, %p:)\n", sym->initializedArray[i], (void*)assign);
			assign->next = list->next;
			assign->chain = list->chain;
			list->next = assign;
			list->chain = assign;
			list = list->next;
		}
	}
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