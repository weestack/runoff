%{
#include "parser.h"
%}

%option yylineno


DIGIT [0-9]

WORD [a-zA-Z][a-zA-Z0-9]*

%%
"#".*\n {
  // should something happen with the comment?
  printf("comment was entered \n"); }

WORD { return IDENTIFIER;}
print {return print;}
[1-9][0-9^a-zA-Z]* {return int_literal;}

[ \t\n\r] ;


%%
