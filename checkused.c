#include <stdio.h>
#include <string.h>
#include "data.h"
#include "auxiliary.h"

static int errors;

static void mark(AstNode *tree){
	AstNode *n;
	AstNode *children;
	for(n = tree; n != NULL; n = n->chain){
		children = n->children;
		if(n->tag == VarDecl)
			children = n->node.VarDecl.expression;
		else if(n->tag == Parameter)
			children = NULL;
		else if(n->tag == DefineFunction && strcmp(n->node.DefineFunction.identifier->node.Identifier.symbol->name, "setup") != 0)
			children = n->children->chain; /* skip the identifier */
		else if(n->tag == DefineTask)
			children = n->children->chain; /* skip the identifier */
		else if(n->tag == Identifier)
			n->node.Identifier.symbol->used++;

		mark(children);
	}
}

static void check(AstNode *tree){
	AstNode *n;
	for(n = tree; n != NULL; n = n->chain){
		if(n->tag == Identifier && n->node.Identifier.symbol->used == 0){
			char *kind;
			Symbol *sym = n->node.Identifier.symbol;
			switch(sym->type->tag){
			case FunctionTypeTag:
				kind = "function";
				break;
			case TaskTypeTag:
				kind = "task";
				break;
			default:
				kind = "variable";
				break;
			}
			eprintf(n->linenum, "Unused %s: %s\n", kind, sym->name);
			errors++;
		}

		if(n->tag != MessageIdentifier)
			check(n->children);
	}
}

int checkused(AstNode *tree){
	mark(tree);
	check(tree);
	return errors;
}
