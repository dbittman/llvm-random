%{
#include <string>
#include "node.h"
#include "lexing.h"

void yyerror(const char *message)
{
	fprintf(stderr, "parse error: %s\n", message);
}

extern NBlock *ast_root;

%}

%union {
	std::string *string;
	NExpression *expr;
	NBlock *block;
	NStatement *stmt;
	NIdentifier *ident;
	std::vector<NExpression*> *exprvec;
	std::vector<NVariableDeclaration*> *varvec;
	NVariableDeclaration *vardecl;
	int token;
}

%debug
%defines
%error-verbose
%token-table
%verbose

%token <token> '+' 
%token <string> TOK_DECIMAL_CONSTANT TOK_IDENT TOK_STRING_CONSTANT TOK_ARROW_RIGHT

%type <block> program stmts block
%type <expr> expr constant lambda
%type <ident> ident
%type <stmt> stmt vardecl func_decl func_prot
%type <exprvec> call_args
%type <varvec> func_decl_args
%start program

%%

program : stmts { ast_root = $1; }
        ;
        
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $$ = $1; $1->statements.push_back($<stmt>2); }
      ;

stmt: vardecl ';' | func_decl | func_prot
	| expr ';' { $$ = new NExpressionStatement(*$1); }
	;

call_args : /*blank*/  { $$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args ',' expr  { $1->push_back($3); }
          ;

vardecl: ident ':' ident '=' expr { $$ = new NVariableDeclaration(*$3, *$1, $5); }
	   | ident ':' ident { $$ = new NVariableDeclaration(*$3, *$1); }
	   ;

expr : expr '+' expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
	 | constant | lambda
	 | ident '(' call_args ')' { $$ = new NMethodCall(*$1, *$3); }
	 | ident { $<ident>$ = $1; }
	 ;

block : '{' stmts '}' { $$ = $2; }
	  | '{' '}' { $$ = new NBlock(); }
	  ;

lambda: '|' func_decl_args '|' TOK_ARROW_RIGHT ident block { $$ = new NLambda(*$5, *new NIdentifier("lambda"), *$2, *$6); }

func_decl : ident ident '(' func_decl_args ')' block 
            { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;

func_prot : ident ident '(' func_decl_args ')' ';'
            { $$ = new NFunctionPrototype(*$1, *$2, *$4); delete $4; }
          ;
    
func_decl_args : /*blank*/  { $$ = new VariableList(); }
          | vardecl { $$ = new VariableList(); $$->push_back($<vardecl>1); }
          | func_decl_args ',' vardecl { $1->push_back($<vardecl>3); }
          ;

ident : TOK_IDENT { $$ = new NIdentifier(*$1); }

constant : TOK_DECIMAL_CONSTANT  { $$ = new NInteger(atol($1->c_str())); delete $1; }
         | TOK_STRING_CONSTANT  { $$ = new NString(*$1); }

%%

