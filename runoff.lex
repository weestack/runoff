%{
#include "parser.h"
%}

%option yylineno

%%
"#".*\n {
  // should something happen with the comment?
  printf("comment was entered \n");

  }



a	{ return a; }
b	{ return b; }
c	{ return c; }


%%
