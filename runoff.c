#include <stdio.h>
#include "parser.h"

int
main(void)
{
	printf("Runoff compiler version: 0\n");
	yyparse(); /* call the parser from yacc */
}