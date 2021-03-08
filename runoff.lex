%{
#include "parser.h"
#include "string.h"
int yyerror(const char*);
%}

%option yylineno


digit [0-9]
integer (0|[1-9]{digit}*)
identifier [a-zA-Z][a-zA-Z0-9]*

%%
"#".*\n {
    // should something happen with the comment?
    printf("comment was entered \n");
}

"const" {return const_keyword;}
; {return ';';}

{integer} {return int_literal;}
{identifier} {return identifier;}
[ \t\n]+ /* skip whitespce, tab, newline and repr */
. {yyerror("Unexpected input");} /* Skip bad chars */

%%
