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

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

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

		virtual llvm::Value* codeGen(CodeGenContext& context) { (void)context; return NULL; }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NIdentifier : public NExpression {
	public:
		std::string name;
		NIdentifier(const std::string& name) : name(name) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NBlock : public NExpression {
	public:
		StatementList statements;
		NBlock() { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NLambda : public NExpression {
	public:
		NIdentifier& type;
		NIdentifier& id;
		VariableList arguments;
		NBlock& block;
		NLambda(NIdentifier& type, NIdentifier& id, 
		        const VariableList& arguments, NBlock& block) :
		    type(type), id(id), arguments(arguments), block(block) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
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
		virtual llvm::Value* codeGen(CodeGenContext& context);
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
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NString : public NExpression {
	public:
		std::string value;
		NString(std::string& value) : value(value) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NInteger : public NExpression {
	public:
		long long value;
		NInteger(long long value) : value(value) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NBinaryOperator : public NExpression {
	public:
		NExpression& lhs;
		NExpression& rhs;
		int op;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		    lhs(lhs), rhs(rhs), op(op) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};
class NExpressionStatement : public NStatement {
	public:
		NExpression& expression;
		NExpressionStatement(NExpression& expression) : 
		    expression(expression) { }
		virtual std::string *to_string(int whitespace = 0);
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
	public:
		NIdentifier& lhs;
		NExpression& rhs;
		NAssignment(NIdentifier& lhs, NExpression& rhs) : 
		    lhs(lhs), rhs(rhs) { }
		virtual std::string *to_string(int whitespace = 0);
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
	public:
		NIdentifier& type;
		NIdentifier& id;
		NExpression *assignmentExpr;
		NVariableDeclaration(NIdentifier& type, NIdentifier& id) :
		    type(type), id(id) { }
		NVariableDeclaration(NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
		    type(type), id(id), assignmentExpr(assignmentExpr) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
		virtual std::string *to_string(int whitespace = 0);
};

class NMethodCall : public NExpression {
	public:
		NIdentifier& id;
		ExpressionList arguments;
		NMethodCall(NIdentifier& id, ExpressionList& arguments) :
		    id(id), arguments(arguments) { }
		NMethodCall(NIdentifier& id) : id(id) { }
		virtual std::string *to_string(int whitespace = 0);
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif

