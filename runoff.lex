%{
#include "parser.h"
#include "string.h"
%}

%option yylineno


DIGIT [0-9]
INTEGER (0|[1-9]{DIGIT}*)
STRING_LITERAL [a-zA-Z][a-zA-Z0-9]*
STRING \"{STRING_LITERAL}*\"

%%
"#".*\n {
    // should something happen with the comment?
    printf("comment was entered \n");
}

"const" {printf("const\n");return CONST;}
"->" {return RIGHT_ARROW;}
";" {printf("end\n");return END;}
"if" {return IF;}
"task" {printf("task\n");return TASK;}
"forever" {return FOREVER;}
"while" {return WHILE;}
"for" {return FOR;}
"{" {return OPEN_BRACKET;}
"}" {return CLOSE_BRACKET;}
"(" {printf("open\n");return OPEN_PARENTHESIS;}
")" {printf("close\n");return CLOSE_PARENTHESIS;}
"," {return COMMA;}
"=" {return EQUAL;}
{INTEGER} {printf("integer_literal\n");return int_literal;}
{STRING_LITERAL} {printf("string_literal\n");return string_literal;}
{STRING} {printf("string\n");return string;}

int_8 {return int_8;}
int_16 {return int_16;}
int_32 {return int_32;}

[ \t\n\r\v\f]+ {} /* skip whitespce, tab, newline and repr */
. {printf("\n");} /* Skip bad chars */

%%
