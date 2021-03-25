#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "symbol.h"
#include "ast.h"
#include "phases.h"

char *filename;

int
main(int argc, char *argv[])
{
	AstNode *tree;

	/* the file name is always the first argument. Rewrite this some day */
	if(argc < 2){
		printf("No input file given\n");
		return -1;
	}
	filename = argv[1];

	tree = parse(filename);
	if(argc >= 3 && strcmp(argv[2], "--prettyprint") == 0){
		if(tree != NULL){
			printf("%s\n", prettyprint(tree));
			return 0; /* early exit */
		}
		else
			printf("Can't prettyprint since I couldn't parse ☹\n");
	}

	if(tree == NULL)
		return -1;
	else
		return buildSymbolTable(tree);
}
