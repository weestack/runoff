/* parse phase defined in runoff.yacc */
AstNode *parse(char *file);

/* pretty print phase defined in prettyprint.c */
char *prettyprint(AstNode *tree);

/* Building of the symbol table defined in buildsymboltable.c */
int buildSymbolTable(AstNode *tree);

/* Checking for type errors defined in typechecker.c */
int typeCheck(AstNode *tree);

/* A phase for other contextual constraints defined in constraints.c */
int contextualConstraintsCheck(AstNode *tree);

/* Make sure the tree contains no variable declarations inside expressions.
   defined in removeNestedDecls.c */
void removeNestedDecls(AstNode *tree);

/* Code generation defined ind codegen.c */
char *codegen(AstNode *tree);