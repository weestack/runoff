%{
#include "parser.h"
%}

%option yylineno


DIGIT [0-9]
ID [a-zA-Z][a-zA-Z0-9]*

%%
"#".*\n {
  // should something happen with the comment?
  printf("comment was entered \n"); }

{ID}+ { return IDENTIFIER;}


print {return print;}
[ \t\n\r] ;


%%
