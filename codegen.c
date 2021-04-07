#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

/* Tænker at hvis codegen returnere en stor streng fra heapen
   lidt i stil med prettyprint, så er det smart, for så kan vi
   selv bestemme om main() i runoff.c skal printe eller putte
   over i en fil eller hvad vi nu vil gøre med strengen. */

char *generateCode(char *code, AstNode *tree);

char *codegen(AstNode *tree){
	/* AstNode *children = getChildren(tree); */
	char *code = malloc(1000000);

	return (generateCode(code, getChildren(tree), nodeLength(tree)));
}

char *generateCode(char *code, AstNode *tree) {
	int i;
	AstNode currentNode = tree;

	switch (tree->tag) {
		case Prog:
		case DefineFunction:
		case DefineTask:
		default:
			strcat(code, smprintf("//[unknown_error] tag: %d", tree->tag));
	}
}
	if (currentNode->node->next != NULL) {
		/* returns next in list */
		return generateCode
	} else if (nodeLength(getChildren(currentNode))) {
		/* returns next level of tree */
		return;
	} else {
		/* returns code string */
		return code;
	}


}
