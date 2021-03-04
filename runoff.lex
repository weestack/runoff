%{
#include "parser.h"
%}

%option yylineno

%%

a	{ return a; }
b	{ return b; }
c	{ return c; }


%%
