#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "symbol.h"
#include "ast.h"
#include "phases.h"

void
prettydummy(void)
{

	AstNode *constnode;
	AstNode *functionparams;
	AstNode *functionbody;
	AstNode *funnode;
	AstNode *tree;

	constnode = mkDefineConstNode(
		mkIdentifierNode("x"),
		mkIntLiteralNode(1234)
	);

	functionparams = mkParameterNode(
		mkBuiltinTypeNode(builtintype_float),
		mkIdentifierNode("a")
	);
	functionparams->next = mkParameterNode(
		mkBuiltinTypeNode(builtintype_int32),
		mkIdentifierNode("x")
	);

	functionbody = mkIfNode(
		mkBinaryOperationNode(
			mkIdentifierNode("a"),
			egreater_equal,
			mkFloatLiteralNode(123.0)
		),
		mkReturnNode(
			mkIntLiteralNode(5.123)
		),
		mkElseNode(
			mkReturnNode(
				mkUnaryOperationNode(
					eincrement,
					postfix,
					mkIdentifierNode("x")
				)
			)
		)
	);

	funnode = mkDefineFunctionNode(
		mkIdentifierNode("testfun"),
		functionparams,
		mkBuiltinTypeNode(builtintype_int32),
		functionbody
	);

	constnode->next = funnode;
	tree = mkProgNode(constnode);

	printf("Pretty print of dummy tree:\n%s\n", prettyprint(tree));

}

int
main(int argc, char *argv[])
{
	AstNode *tree;

	/* the file name is always the first argument. Rewrite this some day */
	if(argc < 2){
		printf("No input file given\n");
		return -1;
	}

	tree = parse(argv[1]);
	if(argc >= 3 && strcmp(argv[2], "--prettyprint") == 0){
		if(tree != NULL)
			printf("%s\n", prettyprint(tree));
		else
			printf("Can't prettyprint since I couldn't parse ☹\n");
	}

	if(tree == NULL)
		return -1;
	else
		return 0;
}
