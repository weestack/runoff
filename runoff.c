#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "parser.h"
#include "symbol.h"
#include "ast.h"
#include "phases.h"
#include "codegen.h"

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

void usage(void){
	printf("Usage: runoff [-p] [-o outfile] filename\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int errors = 0;
	AstNode *tree;
	char *code;

	int ppflag = 0;
	char *outfile = NULL;

	ARGBEGIN{
	case 'p':
		ppflag = 1;
		break;
	case 'o':
		outfile = EARGF(usage());
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
	if(ppflag){
		if(tree != NULL){
			printf("%s\n", prettyprint(tree));
			return 0;
		}else{
			printf("Can't prettyprint since I couln't parse\n");
			return -1;
		}
	}

	errors += buildSymbolTable(tree);
	if(errors != 0)
		return errors;

	errors += typeCheck(tree);
	if(errors != 0)
		return errors;

	/* some code transformations that cannot fail */
	removeNestedDecls(tree);

	/* generate the code! */
	code = codegen(tree);
	if(outfile != NULL){
		FILE *fp = fopen(outfile, "w");
		if (fp != NULL)
			fputs(code, fp);
		else
			printf("Could not open file %s: %s\n", outfile, strerror(errno));
	}else
		printf("%s\n", code);
	return 0;
}
