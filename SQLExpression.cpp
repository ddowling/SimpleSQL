/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLExpression.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Expression Tree
 */
#include <sys/types.h>
#include "SQLExpression.h"
#include "SQLContext.h"
#include <assert.h>

SQLValue SQLExpression::SQLTrueValue(new SQLBooleanValue(true));
SQLValue SQLExpression::SQLFalseValue(new SQLBooleanValue(false));

SQLExpression::SQLExpression()
: refCount(0)
{
}

SQLExpression::~SQLExpression()
{
    assert(refCount == 0);
}

const char * SQLExpression::isA() const
{
    return "SQLExpression";
}

std::string SQLExpression::shortName() const
{
    // Get the full name
    std::string s(isA());

    // Strip off the SQL leader and Expression trailer
    size_t length = s.length();
    std::string leader("SQL");
    std::string trailer("Expression");

    assert(s.substr(0, leader.length()) == leader);
    assert(s.substr(length - trailer.length(), trailer.length()) == trailer);
    assert(length >= trailer.length() + leader.length());

    return s.substr(leader.length(),
                    length - trailer.length() - leader.length());
}

// SQLExpressionList definition
SQLExpressionList::SQLExpressionList()
: numExpr(0), expressions(0)
{
}

SQLExpressionList::~SQLExpressionList()
{
    if (expressions != 0)
    {
	for(int i = 0; i < numExpr; i++)
	    expressions[i]->releaseRef();

	delete[] expressions;
    }
}

void SQLExpressionList::addExpression(SQLExpression *e)
{
    e->getRef();

    SQLExpression **new_expressions =
	new SQLExpression *[numExpr + 1];

    int i;
    for(i = 0; i < numExpr; i++)
	new_expressions[i] = expressions[i];

    new_expressions[i] = e;
    numExpr++;

    if (expressions != 0)
	delete[] expressions;
    expressions = new_expressions;
}

int SQLExpressionList::numExpressions()
{
    return numExpr;
}

SQLExpression * SQLExpressionList::expressionNumber(int i)
{
    if (i < 0 || i >= numExpr)
	return 0;

    return expressions[i];
}

std::string SQLExpressionList::asString() const
{
    std::string str;

    str += "{";

    for(int i = 0; i < numExpr; i++)
    {
	str += expressions[i]->asString();

	if (i < numExpr - 1)
	    str += ", ";
    }

    str += "}";

    return str;
}

// Major expression types
SQLUnaryExpression::SQLUnaryExpression(SQLExpression *expr_)
: expr(expr_)
{
    expr->getRef();
}

SQLUnaryExpression::~SQLUnaryExpression()
{
    // Destroy the children
    expr->releaseRef();
}

std::string SQLUnaryExpression::asString() const
{
    return shortName() + "(" + expr->asString() + ")";
}

const char * SQLUnaryExpression::isA() const
{
    return "SQLUnaryExpression";
}

SQLBinaryExpression::SQLBinaryExpression(SQLExpression *expr1_,
					 SQLExpression *expr2_)
: expr1(expr1_), expr2(expr2_)
{
    expr1->getRef();
    expr2->getRef();
}

SQLBinaryExpression::~SQLBinaryExpression()
{
    // Destroy the children
    expr1->releaseRef();
    expr2->releaseRef();
}

std::string SQLBinaryExpression::asString() const
{
    return shortName() + "(" + expr1->asString() + ", " +
	expr2->asString() + ")";
}

const char * SQLBinaryExpression::isA() const
{
    return "SQLBinaryExpression";
}


bool SQLBinaryExpression::evaluateSiblings(SQLContext &context,
					   SQLValue &v1, SQLValue &v2)
{
    v1 = expr1->evaluate(context);
    // Exception should just return.
    if (v1.isException())
	return false;

    v2 = expr2->evaluate(context);
    if (v2.isException())
    {
	v1 = v2;
	return false;
    }

    if (!v2.typeConvert(v1))
    {
	v1 = SQLValue(new SQLExceptionValue(
                          "Mismatched types in expression: " +
			  v1.asString() + " and " + v2.asString()));
	return false;
    }

    return true;
}

std::string SQLTerminalExpression::asString() const
{
    return shortName();
}

const char * SQLTerminalExpression::isA() const
{
    return "SQLTerminalExpression";
}

// Actual expression implementation
SQLValue SQLEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) == 0);


    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLEqualsExpression::isA() const
{
    return "SQLEqualsExpression";
}

SQLValue SQLNotEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) != 0);


    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLNotEqualsExpression::isA() const
{
    return "SQLNotEqualsExpression";
}

SQLValue SQLLessThanExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) < 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLLessThanExpression::isA() const
{
    return "SQLLessThanExpression";
}

SQLValue SQLGreaterThanExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) > 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLGreaterThanExpression::isA() const
{
    return "SQLGreaterThanExpression";
}

SQLValue SQLLessEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) <= 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLLessEqualsExpression::isA() const
{
    return "SQLLessEqualsExpression";
}

SQLValue SQLGreaterEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) >= 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLGreaterEqualsExpression::isA() const
{
    return "SQLGreaterEqualsExpression";
}

SQLValue SQLAndExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr1->evaluate(context);
    // Exception should just return.
    if (v.isException())
	return v;

    // Shortcut false return
    if (!v.asBoolean())
	return v;

    return expr2->evaluate(context);
}

const char * SQLAndExpression::isA() const
{
    return "SQLAndExpression";
}

SQLValue SQLOrExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr1->evaluate(context);
    // Exception should just return.
    if (v.isException())
	return v;

    // Shortcut true return
    if (v.asBoolean())
	return v;

    return expr2->evaluate(context);
}

const char * SQLOrExpression::isA() const
{
    return "SQLOrExpression";
}

SQLValue SQLOperationExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    return v1.binaryOperation(v2, op);
}

const char * SQLOperationExpression::isA() const
{
    return "SQLOperationExpression";
}

std::string SQLOperationExpression::asString() const
{
    std::string s(1, op);
    return shortName() + "(" + expr1->asString() + " " + s + " " +
	expr2->asString() + ")";
}

SQLValue SQLNotExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);
    // Void or exception should just return.
    if (v.isVoid() || v.isException())
	return v;

    return v.asBoolean() ? SQLFalseValue : SQLTrueValue;
}

const char * SQLNotExpression::isA() const
{
    return "SQLNotExpression";
}

SQLValue SQLNegateExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);

    return v.unaryOperation('-');
}

const char * SQLNegateExpression::isA() const
{
    return "SQLNegateExpression";
}

SQLInExpression::SQLInExpression(SQLExpression *expr_,
				 SQLExpressionList *list_)
: expr(expr_), list(list_)
{
    expr->getRef();
}

SQLInExpression::~SQLInExpression()
{
    delete list;

    expr->releaseRef();
}

SQLValue SQLInExpression::evaluate(SQLContext &context)
{
    SQLValue v1 = expr->evaluate(context);
    // Exception should just return.
    if (v1.isException())
	return v1;

    for (int i = 0; i < list->numExpressions(); i++)
    {
	SQLExpression *e = list->expressionNumber(i);

	SQLValue v2 = e->evaluate(context);
	if (v2.isException())
	    return v2;

	// Convert v2 to the same type as v1
	if (!v2.typeConvert(v1))
	    return SQLValue(new SQLExceptionValue(
				 "Mismatched types in list expression:" +
				 v1.asString() + " and " + v2.asString()));
	if (v1.compare(v2) == 0)
	    return SQLTrueValue;
    }

    return SQLFalseValue;
}

// Show the parse tree as a string. This is useful for debugging
std::string SQLInExpression::asString() const
{
    return shortName() + "(" + expr->asString() + ", " +
	list->asString() + ")";
}

const char * SQLInExpression::isA() const
{
    return "SQLInExpression";
}

SQLLikeExpression::SQLLikeExpression(SQLExpression *expr,
                                     const std::string &pattern,
				     const std::string &escape)
: SQLUnaryExpression(expr)
{
    std::string regexp_str;
    char escape_char = '\0';
    if (escape.length() > 0)
	escape_char = escape[0];

    size_t pos;
    for(pos = 0; pos < pattern.length(); pos++)
    {
	char c = pattern[pos];

	if (c == escape_char)
	{
	    regexp_str += pattern.substr(pos + 1, pattern.length() - pos - 1);
	    break;
	}
	else if (c == '_')
	    regexp_str += ".";
	else if (c ==  '%')
	    regexp_str += ".*";
	else if (c == '+' || c == '*' || c == '?' || c == '.' ||
		 c == '[' || c == ']' || c == '^' || c == '$')
	    regexp_str += "\\" + c;
	else
	    regexp_str.append(&c, 1);
    }

    // Regexp needs to be anchored at the start and end of the string
    regexp_str = "^" + regexp_str + "$";

    regcomp(&regex, regexp_str.c_str(), REG_EXTENDED);
}

SQLLikeExpression::~SQLLikeExpression()
{
    regfree(&regex);
}

SQLValue SQLLikeExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);
    // Exception should just return.
    if (v.isException())
	return v;

    std::string s = v.asString();

    if (regexec(&regex, s.c_str(), 0, 0, 0) == 0)
	return SQLTrueValue;
    else
	return SQLFalseValue;
}

std::string SQLLikeExpression::asString() const
{
    return shortName() + "(" + expr->asString() + ", <regexp>)";
}

const char * SQLLikeExpression::isA() const
{
    return "SQLLikeExpression";
}

SQLFunctionExpression::SQLFunctionExpression(const std::string &class_name,
					     const std::string &member_name,
					     SQLExpressionList *list_)
: className(class_name), memberName(member_name), list(list_)
{
}

SQLFunctionExpression::~SQLFunctionExpression()
{
    delete list;
}

SQLValue SQLFunctionExpression::evaluate(SQLContext &context)
{
    int num_args = list->numExpressions();
    SQLValue *arguments = new SQLValue[num_args];

    for (int i = 0; i < num_args; i++)
    {
	SQLExpression *e = list->expressionNumber(i);

	SQLValue a = e->evaluate(context);

	// If the function arguments are Exceptions
	// then return.
	if (a.isException())
	{
	    delete [] arguments;
	    return a;
	}

	arguments[i] = a;
    }

    SQLValue v = context.functionLookup(className, memberName,
					num_args, arguments);
    delete [] arguments;

    return v;
}

// Show the parse tree as a string. This is useful for debugging
std::string SQLFunctionExpression::asString() const
{
    std::string name;
    if (className.empty())
	name = memberName;
    else
	name = className + "." + memberName;

    return shortName() + "(" + name + ", " + list->asString() + ")";
}

const char * SQLFunctionExpression::isA() const
{
    return "SQLFunctionExpression";
}

SQLValue SQLNullExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);
    // Exception should just return.
    if (v.isException())
	return v;

    return v.isVoid() ? SQLTrueValue : SQLFalseValue;
}

const char * SQLNullExpression::isA() const
{
    return "SQLNullExpression";
}

SQLValue SQLVariableExpression::evaluate(SQLContext &context)
{
    return context.variableLookup(className, memberName);
}

const char * SQLVariableExpression::isA() const
{
    return "SQLVariableExpression";
}

std::string SQLVariableExpression::asString() const
{
    if (className.empty())
	return memberName;
    else
	return className + "." + memberName;
}

SQLValueExpression::SQLValueExpression(SQLValue value_)
: value(value_)
{
}

SQLValue SQLValueExpression::evaluate(SQLContext &)
{
    return value;
}

const char * SQLValueExpression::isA() const
{
    return "SQLValueExpression";
}

std::string SQLValueExpression::asString() const
{
    return value.asString();
}
