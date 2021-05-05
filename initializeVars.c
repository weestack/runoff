#include <stdio.h>
#include <string.h>
#include "data.h"
#include "phases.h"

static void insertInitCode(AstNode *);
static int isInitializedArray(InitializeInfo *, Type *, AstNode *);
static AstNode *getDefaultValue(Type *);
static void initializeStructFields(AstNode *, InitializeInfo *, AstNode *);

void initializeVars(AstNode *tree){
	AstNode *chain = tree->chain;
	if(tree == NULL)
		return;

	if(tree->tag == ExprStmt && tree->node.ExprStmt.expression->tag == VarDecl){
		AstNode *oldchain = tree->chain;
		insertInitCode(tree);
		chain = oldchain;
	}else if(tree->children != NULL)
		initializeVars(tree->children);

	if(chain != NULL)
		initializeVars(chain);
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

static void insertArrayInitCode(InitializeInfo *info, Type *type, AstNode *dims, AstNode *id, AstNode *indices, AstNode *decl){
	if(dims != 0){
		int i;
		int size = dims->node.IntLiteral.value;
		for(i = 0; i < size; i++){
			if(!isInitializedArray(info->arrayInitialized[i], type, dims->next)){
				AstNode *indicesNew;
				indicesNew = extendIndices(indices, i);
				insertArrayInitCode(info->arrayInitialized[i], type, dims->next, id, indicesNew, decl);
			}
		}
	}else{
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

	if(type->tag == ArrayTypeTag && getDefaultValue(type->tags.typeArray.elementType) != NULL)
		insertArrayInitCode(sym->initInfo,
			type->tags.typeArray.elementType,
			type->tags.typeArray.dimensions,
			id, NULL, exprstmt);

	if(type->tag == StructTypeTag && canGetDefaultValue(sym->initInfo, type)){
		AstNode *newid = mkIdentifierNode(id->node.Identifier.identifier);
		newid->node.Identifier.symbol = sym;
		initializeStructFields(exprstmt, sym->initInfo, newid);
	}
}

AstNode *copyIds(AstNode *ids){
	AstNode *newids = NULL;
	AstNode *tmp;
	for(tmp = ids; tmp != NULL; tmp = tmp->next){
		AstNode *newid = mkIdentifierNode(tmp->node.Identifier.identifier);
		newid->node.Identifier.symbol = tmp->node.Identifier .symbol;
		newids = append_node(newids, newid);
	}
	return newids;
}

AstNode *makeStructLocation(AstNode *ids, AstNode *fieldloc){
	ids = copyIds(ids);
	if(ids == NULL)
		return fieldloc;
	else
		return mkStructLocationNode(ids, makeStructLocation(ids->next, fieldloc));
}

void initializeStructFields(AstNode *decl, InitializeInfo *info, AstNode *ids){
	StructInitializeInfo *sinfo;

	for(sinfo=info->structInitialized; sinfo != NULL; sinfo=sinfo->next){
		if(!isInitialized(sinfo->info, sinfo->symbol->type)){
			AstNode *fieldId = mkIdentifierNode(sinfo->symbol->name);
			AstNode *fieldLoc = mkVariableLocationNode(fieldId);
			AstNode *loc = makeStructLocation(ids, fieldLoc);
			AstNode *expr = getDefaultValue(sinfo->symbol->type);
			AstNode *assign = mkExprStmtNode(mkAssignmentNode(loc, expr));

			fieldId->node.Identifier.symbol = sinfo->symbol;

			if(sinfo->symbol->type->tag == StructTypeTag){
				AstNode *newids = copyIds(ids);
				newids = append_node(newids, fieldId);
				initializeStructFields(decl, sinfo->info, newids);
			}else{
				assign->next = decl->next;
				assign->chain = decl->chain;
				assign->parent = decl->parent;
				decl->next = assign;
				decl->chain = assign;
			}
		}
	}
}

static AstNode *getDefaultValue(Type *type){
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

static int isInitializedArray(InitializeInfo *info, Type *t, AstNode *dims){
	if(dims == NULL)
		return isInitialized(info, t);
	else{
		int size = dims->node.IntLiteral.value;
		int i;
		for(i = 0; i < size; i++){
			if(!isInitializedArray(info->arrayInitialized[i], t, dims->next))
				return 0;
		}
		return 1;
	}
}

int isInitialized(InitializeInfo *info, Type *t){
	if(t->tag == BuiltinTypeTag)
		return info->varInitialized;
	else if(t->tag == ArrayTypeTag)
		return isInitializedArray(info,
			t->tags.typeArray.elementType,
			t->tags.typeArray.dimensions);
	else if(t->tag == StructTypeTag){
		StructInitializeInfo *sinfo = info->structInitialized;
		for(; sinfo != NULL; sinfo = sinfo->next){
			if(!isInitialized(sinfo->info, sinfo->symbol->type))
				return 0;
		}
		return 1;
	}else
		return 0;
}

void setInitializedArray(InitializeInfo *info, Type *t, AstNode *dims){
	if(dims == NULL)
		setInitialized(info, t);
	else{
		int size = dims->node.IntLiteral.value;
		int i;
		for(i = 0; i < size; i++)
			setInitializedArray(info->arrayInitialized[i], t, dims->next);
	}
}

void setInitializedStructField(InitializeInfo *info, char *name){
	StructInitializeInfo *sinfo;
	for(sinfo = info->structInitialized; sinfo != NULL; sinfo = sinfo->next){
		if(strcmp(sinfo->symbol->name, name) == 0)
			setInitialized(sinfo->info, sinfo->symbol->type);
	}
}

void setInitialized(InitializeInfo *info, Type *t){
	if(t->tag == BuiltinTypeTag)
		info->varInitialized = 1;
	else if(t->tag == ArrayTypeTag)
		setInitializedArray(info, t->tags.typeArray.elementType, t->tags.typeArray.dimensions);
	else if(t->tag == StructTypeTag){
		Symbol *field;
		for(field = t->tags.typeStruct.fields->symbols; field != NULL; field = field->next)
			setInitializedStructField(info, field->name);
	}
}

int canGetDefaultValue(InitializeInfo *info, Type *t){
	/* check if there exists a default value for type *t.
	   If *t is a struct type, only check the uninitialized fields. */
	if(t->tag == BuiltinTypeTag && getDefaultValue(t) != NULL)
		return 1;
	else if(t->tag == ArrayTypeTag && getDefaultValue(t->tags.typeArray.elementType) != NULL)
		return 1;
	else if(t->tag == StructTypeTag){
		StructInitializeInfo *sinfo = info->structInitialized;
		for(; sinfo != NULL; sinfo = sinfo->next){
			InitializeInfo *fieldinfo = sinfo->info;
			Type *fieldtype = sinfo->symbol->type;
			if(!isInitialized(fieldinfo, fieldtype) && !canGetDefaultValue(fieldinfo, fieldtype))
				return 0;
		}
		return 1;
	}else
		return 0;
}
