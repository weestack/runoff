#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "phases.h"
#include "auxiliary.h"

/* Some code copy pasted from plan9's /sys/include/libc.h
   used to handle command line arguments. Modified to compile
   on non-plan9 machines */
#define	ARGBEGIN	for(argv++,argc--;\
			    argv[0] && argv[0][0]=='-' && argv[0][1];\
			    argc--, argv++) {\
				char *_args, *_argt;\
				char _argc;\
				_args = &argv[0][1];\
				if(_args[0]=='-' && _args[1]==0){\
					argc--; argv++; break;\
				}\
				_argc = 0;\
				while(*_args && (_argc = *_args++))\
				switch(_argc)
#define	ARGEND		}
#define	EARGF(x)	(_argt=_args, _args="",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): ((x), abort(), (char*)0)))

char *filename;

void usage(void);
void verifycode(char *);
char *indent(char *);
void writeFile(char *, char *);
char *readFile(char *);

int
main(int argc, char *argv[])
{
	int errors = 0;
	AstNode *tree;
	char *code;

	int ppflag = 0;
	int verifyflag = 0;
	int indentflag = 0;
	int parseonlyflag = 0;
	char *outfile = NULL;

	ARGBEGIN{
	case 'p':
		ppflag = 1;
		break;
	case 'o':
		outfile = EARGF(usage());
		break;
	case 'v':
		verifyflag = 1;
		break;
	case 'i':
		indentflag = 1;
		break;
	case 'P':
		parseonlyflag = 0;
		break;
	default:
		usage();
	}ARGEND

	/* the file name is the only argument left */
	if(argc != 1)
		usage();
	else
		filename = argv[0];

	tree = parse(filename);
	if(tree == NULL)
		return -1;
	if(parseonlyflag)
		return 0;

	if(ppflag){
		printf("%s\n", prettyprint(tree));
		return 0;
	}

	errors += buildSymbolTable(tree);
	if(errors != 0)
		return errors;

	errors += typeCheck(tree);
	if(errors != 0)
		return errors;

	errors += contextualConstraintsCheck(tree);
	if(errors != 0)
		return errors;

	errors += checkused(tree);
	if(errors != 0)
		return errors;

	/* some code transformations that cannot fail */
	removeNestedDecls(tree);
	initializeVars(tree);

	/* generate the code! */
	code = codegen(tree);

	if(indentflag)
		/* Run indentation helper for output file */
		code = indent(code);

	if(outfile != NULL)
		/* Save output code to file */
		writeFile(code, outfile);
	else
		printf("%s\n", code);

	if(verifyflag)
		/* Run code checker */
		verifycode(code);

	return 0;
}

void usage(void){
	printf("Usage: runoff [-pivP] [-o outfile] filename\n");
	printf("\t-p: prettyprint the code, then exit.\n");
	printf("\t-i: run the generated code through the indent command.\n");
	printf("\t-v: run arduino --verify on the generated code.\n");
	printf("\t-P: stop after parsing.\n");
	printf("\t-o outfile: put the generated code in outfile instead of stdout.\n");
	exit(EXIT_FAILURE);
}

void verifycode(char *code){
	/* wrapper for using arduino c++ to verify output code */
	char *tmpdirname = tmpnam(NULL);
	char *tmpname = smprintf("%s/code.cpp", tmpdirname);
	char *cmd1 = smprintf("mkdir %s", tmpdirname);
	char *cmd2 = smprintf("cd %s && arduino --verify %s", tmpdirname, tmpname);
	system(cmd1);
	writeFile(code, tmpname);
	system(cmd2);
}

char *indent(char *code){
	/* Wrapper for running indent library */
	char *tmpname = tmpnam(NULL);
	char *cmd = smprintf("indent %s", tmpname);
	writeFile(code, tmpname);
	system(cmd);
	return readFile(tmpname);
}
