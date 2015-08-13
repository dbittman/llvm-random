#include <cstdio>
#include "node.h"
#include "llvm_codegen.h"
extern int yyparse();

NBlock *ast_root;
extern int yy_flex_debug, yydebug;
int main(int argc, char **argv)
{
	/* yy_flex_debug = 1; */
	/* yydebug = 1; */
	yyparse();
	CodeGenContext context;
	context.generateCode(*ast_root);
}

