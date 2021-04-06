#include <stdio.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"

/* Tænker at hvis codegen returnere en stor streng fra heapen
   lidt i stil med prettyprint, så er det smart, for så kan vi
   selv bestemme om main() i runoff.c skal printe eller putte
   over i en fil eller hvad vi nu vil gøre med strengen. */
char *codegen(AstNode *tree){
	return smprintf("Good code based on tree %p", tree);
}