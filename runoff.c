#include <stdio.h>
#include "parser.h"
#include "ast.h"

int
main(void)
{
	int ret;
	ret = yyparse(); /* call the parser from yacc */
	return ret;
}
