%{
#include <string>
#include "parsing.h"
#include "yyparse.h"
#include "lexing.h"

#define YY_USER_ACTION  { /* scanner_useraction (); */ }
#define IGNORE(THING)   { }

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(x) yylval.token = x
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
%%

[\t\n ]+ { IGNORE(whitespace); }

"+" { return TOKEN('+'); }
"|" { return TOKEN('|'); }
"," { return TOKEN(','); }
"." { return TOKEN('.'); }
"=" { return TOKEN('='); }
"(" { return TOKEN('('); }
")" { return TOKEN(')'); }
"{" { return TOKEN('{'); }
"}" { return TOKEN('}'); }
"[" { return TOKEN('['); }
"]" { return TOKEN(']'); }
"!" { return TOKEN('!'); }
"@" { return TOKEN('@'); }
"$" { return TOKEN('$'); }
"%" { return TOKEN('%'); }
"^" { return TOKEN('^'); }
"&" { return TOKEN('&'); }
"*" { return TOKEN('*'); }
"~" { return TOKEN('~'); }
";" { return TOKEN(';'); }
":" { return TOKEN(':'); }
"->" { return TOKEN(TOK_ARROW_RIGHT); }

{DECIMAL_NUMBER} { SAVE_TOKEN; return TOK_DECIMAL_CONSTANT; }
{IDENT} { SAVE_TOKEN; return TOK_IDENT; }
{STRING} { yylval.string = new std::string(yytext+1, yyleng-2); return TOK_STRING_CONSTANT; }
. { return 0; }

%%

