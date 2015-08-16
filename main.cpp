#include <iostream>
#include "node.h"
#include "llvm_codegen.h"
extern int yyparse();

NBlock *ast_root;
extern int yy_flex_debug, yydebug;
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	/* yy_flex_debug = 1; */
	/* yydebug = 1; */
	yyparse();

	std::string *ast_dump = ast_root->to_string();

	std::cerr << "AST DUMP:" << std::endl;
	std::cerr << *ast_dump;
	std::cerr << std::endl;

	//CodeGenContext context;
	//context.generateCode(*ast_root);
}

