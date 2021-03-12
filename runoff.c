#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "ast.h"
#include "phases.h"

void
prettydummy(void)
{
	AstNode *tree = NULL;

	printf("Pretty print of dummy tree:\n%s\n", prettyprint(tree));
}

int
main(int argc, char *argv[])
{
	int ret;
	ret = 0;

	if(argc >= 2 && strcmp(argv[1], "--prettyprint") == 0)
		prettydummy(); /* would normally pretty print the AST */
	else
		ret = yyparse(); /* call the parser from yacc */

	return ret;
}
