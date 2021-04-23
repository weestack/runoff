/* From pretttyprint.c */
extern char *operatorNames[];
extern char *builtintypeNames[];

/* from auxiliary.c */
void eprintf(int, char *, ...);
char *smprintf(char *, ...);

/*from typechecker.c*/
Type *typeOf(AstNode *);