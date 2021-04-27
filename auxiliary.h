/* From pretttyprint.c */
extern char *operatorNames[];
extern char *builtintypeNames[];

/* from auxiliary.c */
void eprintf(int, char *, ...);
char *smprintf(char *, ...);
char *readFile(char *);
void writeFile(char *, char *);

/*from typechecker.c*/
Type *typeOf(AstNode *);
