#include "node.h"
#include <string>
#include <typeinfo>
#include <sstream>
#include <boost/lexical_cast.hpp>

std::string *NIdentifier::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += name;
	return ret;
}

std::string *NBlock::to_string(int whitespace)
{
	std::string* ret = new std::string(whitespace, ' ');
	*ret += "(Block)\n";
	for(std::vector<NStatement*>::iterator it = statements.begin(); it != statements.end(); it++) {
		std::string* tmp = ((**it).to_string(whitespace + 4));
		*ret += *tmp;
		*ret += "\n";
	}
	return ret;
}

std::string *NString::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(StrConstant \"" + value + "\")";
	return ret;
}

std::string *NInteger::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(IntConstant ";
	std::stringstream tmp;
	tmp << value;
	*ret += tmp.str();
	*ret += ")";
	return ret;
}
std::string *NAssignment::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Assign)\n";
	*ret += *lhs.to_string(whitespace + 4) + "\n";
	*ret += *rhs.to_string(whitespace + 4);
	return ret;
}

std::string *NExpressionStatement::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Statement)\n";
	*ret += *expr.to_string(whitespace + 4);
	return ret;
}

std::string *NReturn::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Return)\n";
	*ret += *expression.to_string(whitespace + 4);
	return ret;
}

std::string *NVariableDeclaration::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Declare " + *id.to_string() + ")\n";
	*ret += *type.to_string(whitespace + 4);
	return ret;
}

std::string *NTypeArray::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(ArrayType)\n";
	*ret += *type.to_string(whitespace + 4);
	return ret;
}

std::string *NArrayIndex::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(ArrayIndex)\n";
	*ret += *array.to_string(whitespace + 4) + "\n";
	*ret += *index.to_string(whitespace + 4);
	return ret;
}

std::string *NTuple::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Tuple)";
	for(ExpressionList::iterator it = expressions->begin(); it != expressions->end(); it++) {
		*ret += "\n" + *(**it).to_string(whitespace + 4);
	}
	return ret;
}

std::string *NArray::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Array)";
	for(ExpressionList::iterator it = expressions->begin(); it != expressions->end(); it++) {
		*ret += "\n" + *(**it).to_string(whitespace + 4);
	}
	return ret;
}

std::string *NLambda::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Lambda)\n";
	*ret += *type.to_string(whitespace + 2) + "\n";
	*ret += *args.to_string(whitespace + 2) + "\n";
	*ret += *block.to_string(whitespace + 2);
	return ret;
}

std::string *NType::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Type " + name + ")";
	return ret;
}

std::string *NTypeTuple::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(TypeTuple)";
	if(types) {
	for(TypeList::iterator it = types->begin(); it != types->end(); it++) {
		if(*it) {
			*ret += "\n" + *(**it).to_string(whitespace + 4);
		} else {
			*ret += "\n" + std::string(whitespace + 4, ' ') + "(UnknownType)";
		}
	}
	}
	return ret;
}

std::string *NLambdaType::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(TypeLambda)\n";
	*ret += *args.to_string(whitespace + 4) + "\n" + *result.to_string(whitespace + 4);
	return ret;
}

std::string *NBinaryOperator::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(BinOp ";
	if(op > 255)
		*ret += boost::lexical_cast<std::string>(op);
	else
		*ret += (char)op;
	*ret += ")\n";
	*ret += *lhs.to_string(whitespace + 4) + "\n";
	*ret += *rhs.to_string(whitespace + 4);
	return ret;
}

std::string *NUnaryOperator::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(UnOp ";
	*ret += (char)op;
	*ret += ")\n";
	*ret += *rhs.to_string(whitespace + 4);
	return ret;
}

std::string *NMethodCall::to_string(int whitespace)
{
	std::string* ret = new std::string(whitespace, ' ');
	*ret += "(Call " + *id.to_string() + ")\n";
	if(arguments)
		*ret += *arguments->to_string(whitespace + 4);
	else
		*ret += *tupargument->to_string(whitespace + 4);
	return ret;
}


#if 0
std::string *NFunctionDeclaration::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Function ";
	*ret += *id.to_string() + " -> " + *type.to_string();
	*ret += ")\n";
	*ret += std::string(whitespace + 2, ' ');
	*ret += "(Args ";
	for(VariableList::iterator it = arguments.begin(); it != arguments.end(); it++) {
		*ret += *(**it).to_string() + ", ";
	}
	*ret += ")\n";
	*ret += *block.to_string(whitespace + 4);
	return ret;
}

std::string *NFunctionPrototype::to_string(int whitespace)
{
	std::string *ret = new std::string(whitespace, ' ');
	*ret += "(Prototype ";
	*ret += *id.to_string() + " -> " + *type.to_string();
	*ret += ")\n";
	*ret += std::string(whitespace + 2, ' ');
	*ret += "(Args)";
	for(VariableList::iterator it = arguments.begin(); it != arguments.end(); it++) {
		*ret += "\n" + *(**it).to_string(whitespace + 4);
	}
	return ret;
}






#endif

