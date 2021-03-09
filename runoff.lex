%{
#include "parser.h"
#include "string.h"
int yyerror(const char*);
%}

%option yylineno


digit [0-9]
integer -?(0|[1-9]{digit}*)
float {integer}\.{digit}*
identifier [a-zA-Z][a-zA-Z0-9_]*
punctuation [\[\]\(\)\{\};:=,\.\?]


%%
"#".*\n
"->" {return right_arrow;}


"const" {return const_keyword;}
"struct" {return struct_keyword;}
"messages" {return messages;}
"function" {return function;}
"task" {return task;}
"while" {return while_keyword;}
"for" {return for_keyword;}
"elseif" {return elseif_keyword;}
"if" {return if_keyword;}
"else" {return else_keyword;}

"switch" {return switch_keyword;}
"case" {return case_keyword;}
"default" {return default_keyword;}
"receive" {return receive;}
"return" {return return_keyword;}

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
"++" {return increment;}
"--" {return decrement;}
(\+|-|\*|\/|!|<|>) {return yytext[0];}



{identifier} {return identifier;}
[ \t\n]+ /* skip whitespce, tab and newline */
. {yyerror("Unexpected input LEXIS");} /* Skip bad chars */

%%
