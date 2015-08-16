#ifndef __NODE_H
#define __NODE_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NTypeExpression;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<NTypeExpression*> TypeList;
class Node {
	public:
		int character_number;
		int line_number;
		std::string* file_name;
		std::string* string;
		int token;
		virtual ~Node() {}
		virtual std::string *to_string(int whitespace = 0)
		{
			std::string *ret = new std::string(whitespace, ' ');
			*ret += std::string("(Node " + *string + " ");
			*ret += *file_name + ":";
			*ret += line_number + ":";
			*ret += character_number + ")";
			return ret;
		}

		//virtual llvm::Value* codeGen(CodeGenContext& context) { (void)context; return NULL; }
};

class NTypeExpression : public Node {};

class NType : public NTypeExpression {
	public:
		std::string& name;
		NType(std::string &name) : name(name) {}
		virtual std::string *to_string(int whitespace = 0);
};

class NExpression : public Node {
	public:
		NTypeExpression *result_type = NULL;
};

class NStatement : public Node {};


/*class NType : public NTypeExpression {

};*/

class NBlock : public NExpression {
	public:
		StatementList statements;
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NIdentifier : public Node {
	public:
		NIdentifier(std::string &name) : name(name){};
		std::string& name;
		virtual std::string *to_string(int whitespace = 0);
};

class NVariableDeclaration : public NExpression {
	public:
		NIdentifier& id;
		NTypeExpression& type;
		NVariableDeclaration(NIdentifier &id, NTypeExpression &type)
			: id(id), type(type){};
		virtual std::string *to_string(int whitespace = 0);
};

class NTypeTuple : public NTypeExpression {
	public:
		TypeList *types;
		NTypeTuple(TypeList *types) : types(types) {}
		virtual std::string *to_string(int whitespace = 0);
};

class NTuple : public NExpression {
	public:
		ExpressionList *expressions;
		NTypeTuple& type;
		NTuple(NTypeTuple &type) : type(type){};
		virtual std::string *to_string(int whitespace = 0);
};

/*
class NPointer : public NExpression {

};

class NTypePointer: public NTypeExpression {

};
*/

class NExpressionStatement : public NStatement {
	public:
		NExpression& expr;
		NExpressionStatement(NExpression &expr) : expr(expr){};
		virtual std::string *to_string(int whitespace = 0);
};

class NLambdaType : public NTypeExpression {
	public:
		NTypeTuple& args;
		NTypeExpression& result;
		NLambdaType(NTypeTuple &args, NTypeExpression &result) : args(args), result(result){};
		virtual std::string *to_string(int whitespace = 0);
};

class NLambda : public NExpression {
	public:
		NLambdaType& type;
		NTuple& args;
		NBlock& block;
		NLambda(NLambdaType &type, NTuple &args,  NBlock &block)
			: type(type), args(args), block(block){};
		virtual std::string *to_string(int whitespace = 0);
};

class NAssignment : public NExpression {
	public:
		NExpression& lhs;
		NExpression& rhs;
		NAssignment(NExpression &lhs, NExpression &rhs) : lhs(lhs), rhs(rhs){};
		virtual std::string *to_string(int whitespace = 0);
};

class NReturn : public NStatement {
	public:
		NExpression& expression;
		NReturn(NExpression &expression) : expression(expression) {}
		virtual std::string *to_string(int whitespace = 0);
};

class NString : public NExpression {
	public:
		std::string value;
		NString(std::string& value) : value(value) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NTypeArray : public NTypeExpression {
	public:
		NTypeExpression& type;
		NTypeArray(NTypeExpression& type) : type(type) {}
		virtual std::string *to_string(int whitespace = 0);
};

class NArrayIndex : public NExpression {
	public:
		NExpression& array;
		NExpression& index;
		NArrayIndex(NExpression& array, NExpression& index)
			: array(array), index(index) {}
		virtual std::string *to_string(int whitespace = 0);
};

class NInteger : public NExpression {
	public:
		long long value;
		NInteger(long long value) : value(value) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NBinaryOperator : public NExpression {
	public:
		NExpression& lhs;
		NExpression& rhs;
		int op;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		    lhs(lhs), rhs(rhs), op(op) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NUnaryOperator : public NExpression {
	public:
		NExpression& rhs;
		int op;
		NUnaryOperator(int op, NExpression& rhs) :
		    rhs(rhs), op(op) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NMethodCall : public NExpression {
	public:
		NIdentifier& id;
		NTuple *arguments;
		NIdentifier *tupargument;
		NMethodCall(NIdentifier& id, NTuple *arguments) :
		    id(id), arguments(arguments) { }
		NMethodCall(NIdentifier& id, NIdentifier *argument) :
		    id(id), tupargument(argument)
		{
			/* TODO: check if argument is a tuple */

		}
		virtual std::string *to_string(int whitespace = 0);
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
};

#if 0

class NTuple : public NExpression {
	public:
		ExpressionList& expressions;
		NTuple(ExpressionList& expressions) : expressions(expressions) {}
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NType : public NExpression {
	public:
		NExpression& type;
		NType(NExpression& type) : type(type) {}
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NIdentifier : public NExpression {
	public:
		std::string name;
		NIdentifier(const std::string& name) : name(name) { }
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NBlock : public NExpression {
	public:
		StatementList statements;
		NBlock() { }
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NLambda : public NExpression {
	public:
		NExpression& type;
		NIdentifier& id;
		NTuple& arguments;
		NBlock *block;
		NLambda(NTuple& arguments, NIdentifier& id, 
		        NExpression& type) :
		    type(type), id(id), arguments(arguments) { block = NULL; }
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NFunctionDeclaration : public NStatement {
	public:
		NIdentifier& type;
		NIdentifier& id;
		VariableList arguments;
		NBlock& block;
		NFunctionDeclaration(NIdentifier& type, NIdentifier& id, 
		        const VariableList& arguments, NBlock& block) :
		    type(type), id(id), arguments(arguments), block(block) { }
		//virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NFunctionPrototype : public NStatement {
	public:
		NIdentifier& type;
		NIdentifier& id;
		VariableList arguments;
		NFunctionPrototype(NIdentifier& type, NIdentifier& id, 
		        const VariableList& arguments) :
		    type(type), id(id), arguments(arguments) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NString : public NExpression {
	public:
		std::string value;
		NString(std::string& value) : value(value) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NInteger : public NExpression {
	public:
		long long value;
		NInteger(long long value) : value(value) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};


class NExpressionStatement : public NStatement {
	public:
		NExpression& expression;
		NExpressionStatement(NExpression& expression) : 
		    expression(expression) { }
		virtual std::string *to_string(int whitespace = 0);
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
	public:
		NIdentifier& lhs;
		NExpression& rhs;
		NAssignment(NIdentifier& lhs, NExpression& rhs) : 
		    lhs(lhs), rhs(rhs) { }
		virtual std::string *to_string(int whitespace = 0);
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NExpression {
	public:
		NExpression& type;
		NIdentifier& id;
		NExpression *assignmentExpr;
		NVariableDeclaration(NExpression& type, NIdentifier& id) :
		    type(type), id(id) { }
		NVariableDeclaration(NExpression& type, NIdentifier& id, NExpression *assignmentExpr) :
		    type(type), id(id), assignmentExpr(assignmentExpr) { }
	//	virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

#endif
#endif

