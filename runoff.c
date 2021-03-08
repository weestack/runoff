#include <stdio.h>
#include "parser.h"

int
main(void)
{
	int ret;
	printf("Runoff compiler version: 0\n");
	ret = yyparse(); /* call the parser from yacc */
	if (ret == 0)
		printf("Parsed OK\n");
	else
		printf("Parse failed\n");
	return 0;
}
