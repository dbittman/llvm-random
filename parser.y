%{
#include <string>
#include "node.h"
#include "lexing.h"
#include <boost/lexical_cast.hpp>
void yyerror(const char *message)
{
	fprintf(stderr, "parse error: %s\n", message);
}

extern NBlock *ast_root;

%}

%union {
	NExpression *expr;
	NBlock *block;
	NTypeExpression *typeexpr;
	NStatement *stmt;
	NIdentifier *ident;
	std::vector<NExpression*> *exprvec;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NTypeExpression*> *typevec;
	NVariableDeclaration *vardecl;
	Node *node;
}

%debug
%defines
%error-verbose
%token-table
%verbose

%token <node> '+' TOK_DECIMAL_CONSTANT TOK_IDENT TOK_STRING_CONSTANT TOK_ARROW_RIGHT TOK_TYPENAME TOK_RETURN '-' '/' '*' '&' '|' TOK_DOUBLE_EQ TOK_GEQ TOK_LEQ '>' '<' TOK_NEQ

%type <block> program stmts block
%type <expr> expr constant lambda tuple vardecl
%type <typeexpr> type tuple_type lambda_type array_type
%type <typevec> tuple_type_list
%type <ident> ident
%type <stmt> stmt
%type <exprvec> tuple_elems
%start program

%left '<' '>' '=' TOK_NEQ TOK_LEQ TOK_GEQ TOK_DOUBLE_EQ
%left '+' '-' '|'
%left '*' '/' '&'

%nonassoc TOK_ARROW_RIGHT

%%

program : stmts { ast_root = $1; }
        ;
        
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $$ = $1; $1->statements.push_back($<stmt>2); }
      ;

stmt: expr ';' { $$ = new NExpressionStatement(*$1); }
	| TOK_RETURN expr ';' { $$ = new NReturn(*$2); }
	;

vardecl: ident ':' type { $$ = new NVariableDeclaration(*$1, *$3); }
	   ;

expr : constant | lambda
	 | ident tuple { $$ = new NMethodCall(*$1, (NTuple *) $2); }
	 | ident ident { $$ = new NMethodCall(*$1, $2); }
	 | ident { $<ident>$ = $1; }
	 | vardecl
	 | tuple
	 | expr '[' expr ']' { $$ = new NArrayIndex(*$1, *$3); }
	 | expr '=' expr { $$ = new NAssignment(*$1, *$3); }
	 | expr '+' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '-' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '/' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '*' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '|' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '&' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr TOK_DOUBLE_EQ expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr TOK_GEQ expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr TOK_LEQ expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr TOK_NEQ expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '>' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | expr '<' expr { $$ = new NBinaryOperator(*$1, $2->token, *$3); }
	 | '-' expr { $$ = new NUnaryOperator($1->token, *$2); }
	 ;

tuple : '(' tuple_elems ')' {
	NTypeTuple *type = new NTypeTuple(new TypeList());
	NTuple *tuple = new NTuple(*type);
	tuple->expressions = $2;
	for(ExpressionList::iterator it = tuple->expressions->begin();
				it != tuple->expressions->end(); it++) {
		type->types->push_back((**it).result_type);
	}
	$$ = tuple;
}

tuple_elems : { $$ = new ExpressionList(); }
			| expr { $$ = new ExpressionList(); $$->push_back($1); }
			| tuple_elems ',' expr { $1->push_back($3); }
			;
	  
block : '{' stmts '}' { $$ = $2; }
	  | '{' '}' { $$ = new NBlock(); }
	  ;

lambda :  tuple TOK_ARROW_RIGHT type block {
	NLambdaType *type = new NLambdaType(((NTuple*)$1)->type, *$3);
	$$ = new NLambda(*type, (NTuple&)*$1, *$4);
}

ident : TOK_IDENT { $$ = new NIdentifier(*$1->string); }

type : TOK_TYPENAME { $$ = new NType(*$1->string); }
	 | tuple_type
	 | lambda_type
	 | array_type
	 ;

array_type : '[' type ']' { $$ = new NTypeArray(*$2); }

tuple_type : '(' tuple_type_list ')' { NTypeTuple *type = new NTypeTuple(NULL); type->types = $2; $$ = type; }

tuple_type_list : { $$ = new TypeList(); }
				| type { $$ = new TypeList(); $$->push_back($1); }
				| tuple_type_list ',' type { $1->push_back($3); }
				;

lambda_type : tuple_type TOK_ARROW_RIGHT type { $$ = new NLambdaType((NTypeTuple&) *$1, *$3); }

constant : TOK_DECIMAL_CONSTANT  { $$ = new NInteger(atol($1->string->c_str())); delete $1; }
         | TOK_STRING_CONSTANT  { $$ = new NString(*$1->string); }

%%

