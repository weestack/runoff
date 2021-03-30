#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "auxiliary.h"

extern char *filename;

/* printf but it returns a string allocated by malloc */
char *smprintf(char *fmt, ...){
	va_list args;
	char *str = malloc(1000000);
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);
	str = realloc(str, strlen(str) + 1);
	return str;
}

/* print compiler errors of the format "filename:linenum: error" */
void eprintf(int linenum, char *fmt, ...){
	va_list args;
	printf("%s:%d: ", filename, linenum);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}