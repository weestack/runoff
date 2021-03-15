#include <stdio.h>
#include <string.h>
#include "parser.h"
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
	int ret;
	ret = 0;

	if(argc >= 2 && strcmp(argv[1], "--prettyprint") == 0)
		prettydummy(); /* would normally pretty print the AST */
	else
		ret = yyparse(); /* call the parser from yacc */

	return ret;
}
