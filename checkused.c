#include <stdio.h>
#include "symbol.h"
#include "ast.h"
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
		else if(n->tag == Identifier)
			n->node.Identifier.symbol->used++;

		mark(children);
	}
}

static void check(AstNode *tree){
	AstNode *n;
	for(n = tree; n != NULL; n = n->chain){
		if(n->tag == Identifier && n->node.Identifier.symbol->used == 0){
			eprintf(n->linenum, "%s is unused\n", n->node.Identifier.symbol->name);
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