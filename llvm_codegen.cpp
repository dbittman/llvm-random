#include "node.h"
#include "llvm_codegen.h"
#include "parsing.h"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(Node& root)
{
	std::cerr << "Generating code...\n";
	
	/* Create the top level interpreter function to call as entry */
	vector<Type*> argTypes;
	FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
	mainFunction = Function::Create(ftype, GlobalValue::ExternalLinkage, "main", module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	
	/* Push a new variable/block context */
	pushBlock(bblock);
	root.codeGen(*this); /* emit bytecode for the toplevel block */
	ReturnInst::Create(getGlobalContext(), bblock);
	popBlock();
	
	/* Print the bytecode in a human-readable format 
	   to see if our program compiled properly
	 */
	std::cerr << "Code is generated\n";
	PassManager pm;
	pm.add(createPrintModulePass(outs()));
	pm.run(*module);
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NType& type) 
{
/*	if (type.name.compare("int") == 0) {
		return Type::getInt64Ty(getGlobalContext());
	}
	else if (type.name.compare("double") == 0) {
		return Type::getDoubleTy(getGlobalContext());
	}
	else if (type.name.compare("string") == 0) {
		return Type::getInt8Ty(getGlobalContext())->getPointerTo();
	}
	return Type::getVoidTy(getGlobalContext());*/
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context)
{
	(void)context;
	std::cerr << "Creating integer: " << value << std::endl;
	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value* NString::codeGen(CodeGenContext& context)
{
	std::cerr << "Creating string: " << value << std::endl;
	return IRBuilder<> (context.currentBlock()).CreateGlobalStringPtr(value);
}

Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
	std::cerr << "Creating binary operation " << op << std::endl;
	Instruction::BinaryOps instr;
	switch (op) {
		case '+': 	instr = Instruction::Add; goto math;
		//case TMINUS: 	instr = Instruction::Sub; goto math;
		//case TMUL: 		instr = Instruction::Mul; goto math;
		//case TDIV: 		instr = Instruction::SDiv; goto math;
		/* TODO comparison */
	}

	return NULL;
math:
	return BinaryOperator::Create(instr,
			lhs.codeGen(context),
			rhs.codeGen(context),
		"", context.currentBlock());
}

Value* NBlock::codeGen(CodeGenContext& context)
{
	StatementList::const_iterator it;
	Value *last = NULL;
	for (it = statements.begin(); it != statements.end(); it++) {
		std::cerr << "::{} Generating code for " << typeid(**it).name() << std::endl;
		last = (*it)->codeGen(context);
	}
	std::cerr << "Creating block" << std::endl;
	return last;
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
	std::cerr << "Creating assignment for " << lhs.name << std::endl;
	if (context.locals().find(lhs.name) == context.locals().end()) {
		std::cerr << "undeclared variable " << lhs.name << std::endl;
		return NULL;
	}
	return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}


Value* NMethodCall::codeGen(CodeGenContext& context)
{
	Function *function = context.module->getFunction(id.name.c_str());
	if (function == NULL) {
		std::cerr << "no such function " << id.name << std::endl;
		return NULL;
	}
	std::vector<Value*> args;
	ExpressionList::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		args.push_back((**it).codeGen(context));
	}
	CallInst *call = CallInst::Create(function, args, "", context.currentBlock());
	std::cerr << "Creating method call: " << id.name << std::endl;
	return call;
}

Value* NTuple::codeGen(CodeGenContext& context)
{

}

Value* NType::codeGen(CodeGenContext& context)
{

}

Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
	AllocaInst *alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
	context.locals()[id.name] = alloc;
	if (assignmentExpr != NULL) {
		NAssignment assn(id, *assignmentExpr);
		assn.codeGen(context);
	}
	return alloc;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
	std::cerr << ":: Generating code for " << typeid(expression).name() << std::endl;
	return expression.codeGen(context);
}

Value* NIdentifier::codeGen(CodeGenContext& context)
{
	std::cerr << "Creating identifier reference: " << name << std::endl;
	if (context.locals().find(name) == context.locals().end()) {
		std::cerr << "undeclared variable " << name << std::endl;
		return NULL;
	}
	return context.locals()[name];
	//return (new LoadInst(context.locals()[name], "", false, context.currentBlock()))->getPointerOperand();
}

Value* NLambda::codeGen(CodeGenContext& context)
{
	vector<Type*> argTypes;
	VariableList::const_iterator it;
	std::cerr << "Creating lambda: " << id.name << std::endl;
	//for (it = arguments.begin(); it != arguments.end(); it++) {
	//	argTypes.push_back(typeOf((**it).type));
	//}
	FunctionType *ftype = FunctionType::get(typeOf(type), argTypes, false);
	Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

	context.pushBlock(bblock);

	Function::arg_iterator ait;
	int idx=0;
	for(ait = function->arg_begin(); ait != function->arg_end(); ait++) {
		Value *v = ait;
		//auto arg = arguments[idx++];
		//AllocaInst *alloc = new AllocaInst(typeOf(arg->type), arg->id.name.c_str(), context.currentBlock());
		//context.locals()[arg->id.name] = v;
		//v->setName(arg->id.name);
	}
	
	block->codeGen(context);
	ReturnInst::Create(getGlobalContext(), bblock);

	context.popBlock();
	return function;
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
	vector<Type*> argTypes;
	VariableList::const_iterator it;
	std::cerr << "Creating function: " << id.name << std::endl;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		argTypes.push_back(typeOf((**it).type));
	}
	FunctionType *ftype = FunctionType::get(typeOf(type), argTypes, false);
	Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

	context.pushBlock(bblock);

	Function::arg_iterator ait;
	int idx=0;
	for(ait = function->arg_begin(); ait != function->arg_end(); ait++) {
		Value *v = ait;
		auto arg = arguments[idx++];
		//AllocaInst *alloc = new AllocaInst(typeOf(arg->type), arg->id.name.c_str(), context.currentBlock());
		context.locals()[arg->id.name] = v;
		v->setName(arg->id.name);
	}
	
	block.codeGen(context);
	ReturnInst::Create(getGlobalContext(), bblock);

	context.popBlock();
	return function;
}

Value* NFunctionPrototype::codeGen(CodeGenContext& context)
{
	vector<Type*> argTypes;
	VariableList::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		argTypes.push_back(typeOf((**it).type));
	}
	FunctionType *ftype = FunctionType::get(typeOf(type), argTypes, false);
	Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
	//BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

	std::cerr << "Creating function prototype: " << id.name << std::endl;
	return function;
}

