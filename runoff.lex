%{
#include "parser.h"
#include "string.h"
int yyerror(const char*);
%}

%option yylineno


digit [0-9]
integer (0|[1-9]{digit}*)
float {integer}\.{digit}*
identifier [a-zA-Z][a-zA-Z0-9_]*
punctuation [\(\)\{\};:=,]


%%
"#".*\n
"->" {return right_arrow;}


"const" {return const_keyword;}
"function" {return function;}
"task" {return task;}
"while" {return while_keyword;}
"for" {return for_keyword;}

{punctuation} {return yytext[0];}


{integer} {return int_literal;}
{float} {return float_literal;}
("true"|"false") {return bool_literal;}

(u?int(8|16|32|64)?|float|bool|void|msg) {return builtin_type;}



"&&" {return and_op;}
"||" {return or_op;}
">=" {return greater_equal;}
"<=" {return less_equal;}
"==" {return equal;}
(\+|-|\*|\/|!|<|>) {return yytext[0];}



{identifier} {return identifier;}
[ \t\n]+ /* skip whitespce, tab and newline */
. {yyerror("Unexpected input LEXIS");} /* Skip bad chars */

%%
