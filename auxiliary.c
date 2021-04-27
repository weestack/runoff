#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

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

/* open and write buf to file */
void writeFile(char *buf, char *filename){
	FILE *fp = fopen(filename, "w");
	if (fp != NULL){
		fputs(buf, fp);
		fflush(fp);
		fclose(fp);
	}
	else
		printf("Could not open file %s: %s\n", filename, strerror(errno));
}

/* read the contents of file */
char *readFile(char *filename){
	FILE *fp;
	char *buf;
	long length;
	fp = fopen(filename, "r");
	if(fp == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = malloc(length+1);
	if(buf == NULL)
		return NULL;
	fread(buf, 1, length, fp);
	buf[length] = '\0';
	fclose(fp);
	return buf;
}