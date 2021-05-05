%{
#include <strings.h>
#include "parser.h"
#include "data.h"
int yyerror(const char*);
%}

%option yylineno


digit [0-9]
integer -?(0|[1-9]{digit}*)
base_16 -?0(x|X)[0-9a-fA-F]*
float {integer}\.{digit}*
identifier [a-zA-Z][a-zA-Z0-9_]*
punctuation [\[\]\(\)\{\};:=,\.\?]


%%
"#".*\n
"->" {return right_arrow;}

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
"spawn" {return spawn_keyword;}
"send" {return send_keyword;}
"to" {return to_keyword;}

{punctuation} {return yytext[0];}

{integer}P {
	yylval.astNode = mkPinLiteralNode(strtoll(yytext, NULL, 10));
	return pin_literal;
}

{integer} {
	yylval.astNode = mkIntLiteralNode(strtoll(yytext, NULL, 10));
	return int_literal;
}
{base_16} {
    yylval.astNode = mkIntLiteralNode(strtoll(yytext, NULL, 16));
    return int_literal;
}
{float} {
	yylval.astNode = mkFloatLiteralNode(strtof(yytext, NULL));
	return float_literal;
}
"true" {yylval.astNode = mkBoolLiteralNode(1); return bool_literal;}
"false" {yylval.astNode = mkBoolLiteralNode(0); return bool_literal;}

uint8	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeUint8); return builtin_type;}
uint16	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeUint16); return builtin_type;}
uint32	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeUint32); return builtin_type;}
uint64	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeUint64); return builtin_type;}
int8	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeInt8); return builtin_type;}
int16	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeInt16); return builtin_type;}
int32	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeInt32); return builtin_type;}
int64	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeInt64); return builtin_type;}
int	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeInt32); return builtin_type;}
float	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeFloat); return builtin_type;}
void	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeVoid); return builtin_type;}
bool	{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeBool); return builtin_type;}
msg		{yylval.astNode = mkBuiltinTypeNode(BuiltinTypeMsg); return builtin_type;}
taskid  {yylval.astNode = mkBuiltinTypeNode(BuiltinTypeTaskid); return builtin_type;}
pinid {yylval.astNode = mkBuiltinTypeNode(BuiltinTypePinid); return builtin_type;}

"&&" {return and_op;}
"||" {return or_op;}
">=" {return greater_equal;}
"<=" {return less_equal;}
"==" {return equal;}
"++" {return increment;}
"!=" {return not_equal;}
"--" {return decrement;}

"&" {return bitwise_and;}
"|" {return bitwise_or;}
"^" {return bitwise_xor;}
"~" {return bitwise_not;}
">>" {return right_shift;}
"<<" {return left_shift;}

"<" {return less_than;}
">" {return greater_than;}
"%" {return modulo;}
(\+|-|\*|\/|!) {return yytext[0];}

{identifier} {yylval.astNode = mkIdentifierNode(strdup(yytext)); return identifier;}
[ \t\n\r]+ /* skip whitespce, tab and newline */

. {yyerror("Unexpected input in lexer");} /* error on unexpected input */

%%
