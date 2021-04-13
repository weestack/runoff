#include <stdio.h>
#include <string.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

static int errors = 0;
static void checkTreeHasSetup(AstNode *tree);
static void checkSwitchHasDefault(AstNode *tree);
static void checkReceiveHasDefault(AstNode *tree);
static void checkNotGlobalVar(AstNode *tree);
static void checkHasMaxOneMessageBlock(AstNode *tree);

/* The following list of contextual constraints are checked by
   this phase:
   1) Each tree has one setup function with type setup(void)->void
   2) Each switch has exactly one default case
   3) Each receive has exactly one default case
   4) No modification of global variables
   5) Each tree has at most one messages block
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
		if(tree->node.UnaryOperation.operator == edecrement
			|| tree->node.UnaryOperation.operator == eincrement)
			checkNotGlobalVar(tree->node.UnaryOperation.expression);
		break;
	}
	
	children = getChildren(tree);
	while(children != NULL){
		contextualConstraintsCheck(children);
		children = children->next;
	}
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
			&& t.returnType->tags.typeBuiltin.builtinType == builtintype_void)
			return;
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