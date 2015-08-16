%{
#include <string>
#include "parsing.h"
#include "yyparse.h"
#include "lexing.h"

int lexer_character_number = 0;
int lexer_line_number = 0;
std::string *lexer_file_name = new std::string("stdin");

void save_token(Node *node)
{
	node->string = new std::string(yytext, yyleng);
	node->character_number = lexer_character_number;
	node->line_number = lexer_line_number;
	node->file_name = lexer_file_name;
}

#define YY_USER_ACTION  { lexer_character_number += yyleng; yylval.node = new Node(); save_token(yylval.node); }
#define IGNORE(THING)   { }

#define SAVE_TOKEN save_token(yylval.node)

#define TOKEN(x) yylval.node->token = x
%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

DECIMAL_NUMBER ([1-9])([0-9])*
IDENT [a-zA-Z_][a-zA-Z0-9_]*
STRING \"[^\"]+\"
TYPE %{IDENT}
%%

[\t ]+ { IGNORE(whitespace); }
\n { lexer_line_number++; lexer_character_number = 0; }

"+" { SAVE_TOKEN; return TOKEN('+'); }
"-" { SAVE_TOKEN; return TOKEN('-'); }
"/" { SAVE_TOKEN; return TOKEN('/'); }
"|" { SAVE_TOKEN; return TOKEN('|'); }
"," { SAVE_TOKEN; return TOKEN(','); }
"." { SAVE_TOKEN; return TOKEN('.'); }
"=" { SAVE_TOKEN; return TOKEN('='); }
"(" { SAVE_TOKEN; return TOKEN('('); }
")" { SAVE_TOKEN; return TOKEN(')'); }
"{" { SAVE_TOKEN; return TOKEN('{'); }
"}" { SAVE_TOKEN; return TOKEN('}'); }
"[" { SAVE_TOKEN; return TOKEN('['); }
"]" { SAVE_TOKEN; return TOKEN(']'); }
"!" { SAVE_TOKEN; return TOKEN('!'); }
"@" { SAVE_TOKEN; return TOKEN('@'); }
"$" { SAVE_TOKEN; return TOKEN('$'); }
"%" { SAVE_TOKEN; return TOKEN('%'); }
"^" { SAVE_TOKEN; return TOKEN('^'); }
"&" { SAVE_TOKEN; return TOKEN('&'); }
"*" { SAVE_TOKEN; return TOKEN('*'); }
"~" { SAVE_TOKEN; return TOKEN('~'); }
";" { SAVE_TOKEN; return TOKEN(';'); }
":" { SAVE_TOKEN; return TOKEN(':'); }
">" { SAVE_TOKEN; return TOKEN('>'); }
"<" { SAVE_TOKEN; return TOKEN('<'); }
"==" { SAVE_TOKEN; return TOKEN(TOK_DOUBLE_EQ); }
">=" { SAVE_TOKEN; return TOKEN(TOK_GEQ); }
"<=" { SAVE_TOKEN; return TOKEN(TOK_LEQ); }
"!=" { SAVE_TOKEN; return TOKEN(TOK_NEQ); }
"return" { SAVE_TOKEN; return TOKEN(TOK_RETURN); }
"->" { SAVE_TOKEN; return TOKEN(TOK_ARROW_RIGHT); }

{DECIMAL_NUMBER} { SAVE_TOKEN; return TOK_DECIMAL_CONSTANT; }
{IDENT} { SAVE_TOKEN; return TOK_IDENT; }
{TYPE} { SAVE_TOKEN; return TOK_TYPENAME; }
{STRING} { SAVE_TOKEN; /* MEMORY LEAK */ yylval.node->string = new std::string(yytext+1, yyleng-2); return TOK_STRING_CONSTANT; }
. { return 0; }

%%

