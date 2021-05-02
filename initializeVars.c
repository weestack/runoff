#include <stdio.h>
#include "data.h"
#include "phases.h"

static void insertInitCode(AstNode *);

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

static AstNode *extendIndices(AstNode *indices, int i){
	AstNode *result = NULL;
	AstNode *tmp;
	for(tmp = indices; tmp != NULL; tmp = tmp->next){
		AstNode *n = mkIntLiteralNode(tmp->node.IntLiteral.value);
		result = append_node(result, n);
	}
	result = append_node(result, mkIntLiteralNode(i));
	return result;
}

static void insertArrayInitCode(InitializeInfo *info, Type *type, AstNode *id, AstNode *indices, AstNode *decl){
	decl = decl;
	if(type->tag == ArrayTypeTag){
		int i;
		for(i = 0; i < type->tags.typeArray.size; i++){
			if(!isInitialized(info->arrayInitialized[i], type->tags.typeArray.elementType)){
				AstNode *indicesNew;
				indicesNew = extendIndices(indices, i);
				insertArrayInitCode(info->arrayInitialized[i], type->tags.typeArray.elementType, id, indicesNew, decl);
			}
		}
	}else if(type->tag == BuiltinTypeTag){
		AstNode *loc = mkArrayLocationNode(id, indices);
		AstNode *expr = getDefaultValue(type);
		AstNode *assign = mkExprStmtNode(mkAssignmentNode(loc, expr));
		assign->next = decl->next;
		assign->chain = decl->chain;
		assign->parent = decl->parent;
		decl->next = assign;
		decl->chain = assign;
	}
}

static void insertInitCode(AstNode *exprstmt){
	AstNode *decl = exprstmt->node.ExprStmt.expression;
	AstNode *id = decl->node.VarDecl.identifier;
	Symbol *sym = id->node.Identifier.symbol;
	Type *type = sym->type;

	if(type->tag == BuiltinTypeTag && !isInitialized(sym->initInfo, type))
		decl->node.VarDecl.expression = getDefaultValue(type);

	if(type->tag == ArrayTypeTag && getDefaultValue(arrayBaseType(type)) != NULL)
		insertArrayInitCode(sym->initInfo, type, id, NULL, exprstmt);
/*
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
*/
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

int isInitialized(InitializeInfo *info, Type *t){
	if(t->tag == BuiltinTypeTag)
		return info->varInitialized;
	else if(t->tag == ArrayTypeTag){
		int i;
		for(i = 0; i < t->tags.typeArray.size; i++){
			if(!isInitialized(info->arrayInitialized[i], t->tags.typeArray.elementType))
				return 0;
		}
		return 1;
	}else if(t->tag == StructTypeTag)
		return 0; /* haha */
	else
		return 0;
}

void setInitialized(InitializeInfo *info, Type *t){
	if(t->tag == BuiltinTypeTag)
		info->varInitialized = 1;
	else if(t->tag == ArrayTypeTag){
		int i;
		for(i = 0; i < t->tags.typeArray.size; i++){
			setInitialized(info->arrayInitialized[i], t->tags.typeArray.elementType);
		}
	}
}
