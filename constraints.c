#include <stdio.h>
#include <string.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

static int errors = 0;
static void checkTreeHasSetup(AstNode *tree);

/* The following list of contextual constraints are checked by
   this phase:
   1) Each tree has one setup function with type setup(void)->void
 */
int contextualConstraintsCheck(AstNode *tree){
	AstNode *children;

	switch(tree->tag){
	case Prog:
		checkTreeHasSetup(tree);
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