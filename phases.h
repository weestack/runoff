/* parse phase defined in runoff.yacc */
AstNode *parse(char *file);

/* pretty print phase defined in prettyprint.c */
char *prettyprint(AstNode *tree);

/* Building of the symbol table defined in buildsymboltable.c */
int buildSymbolTable(AstNode *tree);

/* Checking for type errors defined in typechecker.c */
int typeCheck(AstNode *tree);

/* Code generation defined ind codegen.c */
char *codegen(AstNode *tree);