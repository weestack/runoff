/* parse phase defined in runoff.yacc */
AstNode *parse(char *file);

/* pretty print phase defined in prettyprint.c */
char *prettyprint(AstNode *tree);

/* Building of the symbol table defined in buildsymboltable.c */
int buildSymbolTable(AstNode *tree);
