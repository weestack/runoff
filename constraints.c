#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "auxiliary.h"

static int errors = 0;
static void checkTreeHasSetup(AstNode *tree);
static void checkSwitchHasDefault(AstNode *tree);
static void checkReceiveHasDefault(AstNode *tree);
static void checkNotGlobalVar(AstNode *tree);
static void checkHasMaxOneMessageBlock(AstNode *tree);
static int entireArrayInitialized(Symbol *sym);
static void checkVarInitialized(AstNode *tree);
static void checkAllSpawnsInSetup(AstNode *prog, AstNode *stmts);
static int countSpawns(AstNode *nodes, int recurse);
static void checkAllAdvancedInputInSetup(AstNode *prog, AstNode *stmts);
static int countFunctioncall(char *name, AstNode *nodes, int recurse);

/* The following list of contextual constraints are checked by
   this phase:
   1) Each tree has one setup function with type setup(void)->void
   2) Each switch has exactly one default case
   3) Each receive has exactly one default case
   4) No modification of global variables
   5) Each tree has at most one messages block
   6) All variables are initialized before use, or have a default value
   7) All spawns appear as a statement directly in the setup function
   8) All calls to advancedInputPin appear as a statement directly in the setup function
 */
int contextualConstraintsCheck(AstNode *tree){
	AstNode *children;
	switch(tree->tag){
	case Prog:
		checkTreeHasSetup(tree);
		checkHasMaxOneMessageBlock(tree);
		break;
	case Switch:
		checkSwitchHasDefault(tree);
		break;
	case Receive:
		checkReceiveHasDefault(tree);
		break;
	case Assignment:
		checkNotGlobalVar(tree->node.Assignment.location);
		break;
	case UnaryOperation:
		if(tree->node.UnaryOperation.operator == OpDecrement
			|| tree->node.UnaryOperation.operator == OpIncrement)
			checkNotGlobalVar(tree->node.UnaryOperation.expression);
		break;
	case VariableLocation:
	case StructLocation:
	case ArrayLocation:
		checkVarInitialized(tree);
		return errors;
	}

	children = tree->children;
	for(; children != NULL; children = children->chain)
		contextualConstraintsCheck(children);
	return errors;
}

/* Look at all the toplevel declarations and see if a function
   with the signature 'setup() -> void' exists */
static void checkTreeHasSetup(AstNode *tree){
	AstNode *toplevels = tree->node.Prog.toplevels;
	AstNode *node;
	for(node = toplevels; node != NULL; node = node->next){
		Symbol *s;
		FunctionTypeDescriptor t;
		if(node->tag != DefineFunction)
			continue;

		s = node->node.DefineFunction.identifier->node.Identifier.symbol;
		if(strcmp("setup", s->name) != 0)
			continue;

		if(s->type->tag != FunctionTypeTag)
			continue;

		t = s->type->tags.typeFunction;
		if(t.arity == 0
			&& t.returnType->tag == BuiltinTypeTag
			&& t.returnType->tags.typeBuiltin.builtinType == BuiltinTypeVoid){
			checkAllSpawnsInSetup(tree, node->node.DefineFunction.statements);
			checkAllAdvancedInputInSetup(tree, node->node.DefineFunction.statements);
			return;
		}
	}
	eprintf(1, "No setup function with type 'setup() -> void' found in program\n");
	errors++;
}

/* go through all cases and check if there is exactly one default case */
static void checkSwitchHasDefault(AstNode *tree){
	AstNode *cases = tree->node.Switch.cases;
	AstNode *node;
	int defaultCount = 0;
	for(node = cases; node != NULL; node = node->next){
		if(node->node.SwitchCase.literal == NULL)
			defaultCount++;
	}

	if(defaultCount != 1){
		errors++;
		eprintf(tree->linenum, "Switch statements must have exactly one default case. This one has %d\n", defaultCount);
	}
}

/* go through all cases and check if there is exactly one default case */
static void checkReceiveHasDefault(AstNode *tree){
	AstNode *cases = tree->node.Receive.cases;
	AstNode *node;
	int defaultCount = 0;
	for(node = cases; node != NULL; node = node->next){
		if(node->node.ReceiveCase.messageName == NULL)
			defaultCount++;
	}

	if(defaultCount != 1){
		errors++;
		eprintf(tree->linenum, "Receive statements must have exactly one default case. This one has %d\n", defaultCount);
	}
}

/* Check if the tree is a location, and if so, check that it isn't global */
static void checkNotGlobalVar(AstNode *tree){
	AstNode *id;
	switch(tree->tag){
	case VariableLocation:
		id = tree->node.VariableLocation.identifier;
		break;
	case StructLocation:
		id = tree->node.StructLocation.identifier;
		break;
	case ArrayLocation:
		id = tree->node.ArrayLocation.identifier;
		break;
	default:
		return;
	}

	if(id->node.Identifier.symbol->globalvar){
		eprintf(tree->linenum, "Cannot modify the global variable '%s'\n", id->node.Identifier.symbol->name);
		errors++;
	}
}

/* Check that there is at most one messages block in the program */
static void checkHasMaxOneMessageBlock(AstNode *tree){
	int msgblockCount = 0;
	AstNode *toplevels = tree->node.Prog.toplevels;
	AstNode *node;
	for(node = toplevels; node != NULL; node = node->next){
		if(node->tag == DefineMessage)
			msgblockCount++;
	}

	if(msgblockCount > 1){
		eprintf(1, "At most one messages block is allowed per program. This program has %d\n", msgblockCount);
		errors++;
	}
}

/* check that the variable is either initialized before use, or have a default value */
static void checkVarInitialized(AstNode *tree){
	AstNode *id;
	Symbol *sym;
	Type *type;
	switch(tree->tag){
	case VariableLocation:
		id = tree->node.VariableLocation.identifier;
		break;
	case StructLocation:
		id = tree->node.StructLocation.identifier;
		break;
	case ArrayLocation:
		id = tree->node.ArrayLocation.identifier;
		break;
	default:
		return;
	}

	sym = id->node.Identifier.symbol;
	type = sym->type;
	if(type->tag == BuiltinTypeTag && !sym->initializedVar){
		AstNode *expr = getDefaultValue(type);
		if(expr == NULL){
			eprintf(tree->linenum, "Variable '%s' is not initialized when used here\n", sym->name);
			errors++;
		}
	}else if(type->tag == ArrayTypeTag && !entireArrayInitialized(sym) && tree->parent->tag != Assignment){
		int i = 0;
		AstNode *expr = getDefaultValue(type->tags.typeArray.elementType);
		if(expr == NULL){
			eprintf(tree->linenum, "Warning: Elements of array '%s' might not be initialized at indices: ", sym->name);
			for(i = 0; i < type->tags.typeArray.size; i++){
				if(sym->initializedArray[i] == 0)
					printf("%d ", i);
			}
			printf("\n");
		}
	}
}

static int entireArrayInitialized(Symbol *sym){
	int i;
	for(i = 0; i < sym->type->tags.typeArray.size; i++){
		if(sym->initializedArray[i] == 0)
			return 0;
	}
	return 1;
}

static int countSpawns(AstNode *nodes, int recurse){
	AstNode *n;
	int count = 0;
	for(n = nodes; n != NULL; n = n->chain){
		if(n->tag == Spawn)
			count++;
		if(recurse || n->tag == ExprStmt || n->tag == VarDecl || n->tag == Assignment)
			count += countSpawns(n->children, recurse);
	}
	return count;
}

static void checkAllSpawnsInSetup(AstNode *prog, AstNode *stmts){
	int spawncountall = countSpawns(prog->node.Prog.toplevels, 1);
	int spawncountstmts = countSpawns(stmts, 0);
	prog->node.Prog.spawnCount = spawncountstmts;
	if(spawncountall > spawncountstmts){
		errors++;
		eprintf(1, "It looks like you have spawns located somewhere else than directly in the setup function, which is not allowed.\n");
	}
}

static int countFunctioncall(char *name, AstNode *nodes, int recurse){
	AstNode *n;
	int count = 0;
	for(n = nodes; n != NULL; n = n->chain){
		if(n->tag == FunctionCall && strcmp(n->node.FunctionCall.identifier->node.Identifier.symbol->name, name) == 0)
			count++;
		if(recurse || n->tag == ExprStmt)
			count += countFunctioncall(name, n->children, recurse);
	}
	return count;
}

static void checkAllAdvancedInputInSetup(AstNode *prog, AstNode *stmts){
	int countall = countFunctioncall("advancedInputPin", prog->node.Prog.toplevels, 1);
	int countstmts = countFunctioncall("advancedInputPin", stmts, 0);
	if(countall > countstmts){
		errors++;
		eprintf(1, "It looks like you have calls to advancedInputPin located somewhere else than directly in the setup function, which is not allowed.\n");
	}
}