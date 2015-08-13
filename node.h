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
		virtual ~Node() {}
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
};

class NBlock : public NExpression {
	public:
		StatementList statements;
		NBlock() { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NLambda : public NExpression {
	public:
		NIdentifier& type;
		const NIdentifier& id;
		VariableList arguments;
		NBlock& block;
		NLambda(NIdentifier& type, const NIdentifier& id, 
		        const VariableList& arguments, NBlock& block) :
		    type(type), id(id), arguments(arguments), block(block) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
	public:
		NIdentifier& type;
		const NIdentifier& id;
		VariableList arguments;
		NBlock& block;
		NFunctionDeclaration(NIdentifier& type, const NIdentifier& id, 
		        const VariableList& arguments, NBlock& block) :
		    type(type), id(id), arguments(arguments), block(block) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionPrototype : public NStatement {
	public:
		NIdentifier& type;
		const NIdentifier& id;
		VariableList arguments;
		NFunctionPrototype(NIdentifier& type, const NIdentifier& id, 
		        const VariableList& arguments) :
		    type(type), id(id), arguments(arguments) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NString : public NExpression {
	public:
		std::string value;
		NString(std::string& value) : value(value) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NInteger : public NExpression {
	public:
		long long value;
		NInteger(long long value) : value(value) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
	public:
		NExpression& lhs;
		NExpression& rhs;
		int op;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		    lhs(lhs), rhs(rhs), op(op) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NExpressionStatement : public NStatement {
	public:
		NExpression& expression;
		NExpressionStatement(NExpression& expression) : 
		    expression(expression) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
	public:
		const NIdentifier& lhs;
		NExpression& rhs;
		NAssignment(const NIdentifier& lhs, NExpression& rhs) : 
		    lhs(lhs), rhs(rhs) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		const NIdentifier& id;
		NExpression *assignmentExpr;
		NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
		    type(type), id(id) { }
		NVariableDeclaration(const NIdentifier& type, const NIdentifier& id, NExpression *assignmentExpr) :
		    type(type), id(id), assignmentExpr(assignmentExpr) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
	public:
		const NIdentifier& id;
		ExpressionList arguments;
		NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
		    id(id), arguments(arguments) { }
		NMethodCall(const NIdentifier& id) : id(id) { }
		virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif

