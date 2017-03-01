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
#include <string.h>
#include <stdio.h>

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
    return std::string(shortName()) + "(" + expr->asString() + ")";
}

const char * SQLUnaryExpression::shortName() const
{
    return "nUary";
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
    return std::string(shortName()) + "(" + expr1->asString() + ", " +
	expr2->asString() + ")";
}

const char * SQLBinaryExpression::shortName() const
{
    return "Binary";
}


bool SQLBinaryExpression::evaluateSiblings(SQLContext &context,
					   SQLValue &v1, SQLValue &v2)
{
    v1 = expr1->evaluate(context);
    // Exception should just return.
    if (v1.isException() || v1.isNull())
        return false;

    // Performance optimisation to aim to only do the type conversion once
    // for things like SQLDateTimeValue
    SQLValueExpression *ve = dynamic_cast<SQLValueExpression *>(expr2);
    if (ve != 0)
        v2 = ve->evaluateAsType(v1);
    else
        v2 = expr2->evaluate(context);

    if (v2.isException() || v2.isNull())
    {
	v1 = v2;
	return false;
    }

    if (v2.isSameType(v1))
        return true;

    if (v2.typeConvert(v1))
        return true;

    v1 = SQLValue(new SQLExceptionValue(
                      "Mismatched types in expression: " +
                      v1.asString() + " and " + v2.asString()));
    return false;
}

std::string SQLTerminalExpression::asString() const
{
    return shortName();
}

const char * SQLTerminalExpression::shortName() const
{
    return "Terminal";
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

const char * SQLEqualsExpression::shortName() const
{
    return "Equals";
}

SQLValue SQLNotEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) != 0);


    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLNotEqualsExpression::shortName() const
{
    return "NotEquals";
}

SQLValue SQLLessThanExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) < 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLLessThanExpression::shortName() const
{
    return "LessThan";
}

SQLValue SQLGreaterThanExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) > 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLGreaterThanExpression::shortName() const
{
    return "GreaterThan";
}

SQLValue SQLLessEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) <= 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLLessEqualsExpression::shortName() const
{
    return "LessEquals";
}

SQLValue SQLGreaterEqualsExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    bool res = (v1.compare(v2) >= 0);

    return res ? SQLTrueValue : SQLFalseValue;
}

const char * SQLGreaterEqualsExpression::shortName() const
{
    return "GreaterEquals";
}

SQLValue SQLAndExpression::evaluate(SQLContext &context)
{
    SQLValue v1 = expr1->evaluate(context);
    // Exception should just return.
    if (v1.isException())
	return v1;

    // Shortcut false return. Don't shortcut on void as we want this to
    // walk the entire expression tree.
    if (!v1.isNull() && !v1.asBoolean())
	return v1;

    SQLValue v2 = expr2->evaluate(context);
    if (!v2.asBoolean() || v2.isNull())
	return v2;
    else
	return v1;
}

const char * SQLAndExpression::shortName() const
{
    return "And";
}

SQLValue SQLOrExpression::evaluate(SQLContext &context)
{
    SQLValue v1 = expr1->evaluate(context);
    // Exception should just return.
    if (v1.isException())
	return v1;

    // Shortcut true return
    if (v1.asBoolean())
	return v1;

    SQLValue v2 = expr2->evaluate(context);
    if (v2.asBoolean() || v2.isNull())
	return v2;
    else
	return v1;
}

const char * SQLOrExpression::shortName() const
{
    return "Or";
}

SQLValue SQLXorExpression::evaluate(SQLContext &context)
{
    SQLValue v1 = expr1->evaluate(context);
    SQLValue v2 = expr2->evaluate(context);
    // Exception should just return.
    if (v1.isException())
       return v1;
    if (v2.isException())
       return v2;

    bool b1 = v1.asBoolean();
    bool b2 = v2.asBoolean();
    if (b1 ^ b2)
       return SQLTrueValue;
    else
       return SQLFalseValue;
}

const char * SQLXorExpression::shortName() const
{
    return "Xor";
}

SQLValue SQLOperationExpression::evaluate(SQLContext &context)
{
    SQLValue v1, v2;
    if (!evaluateSiblings(context, v1, v2))
	return v1;

    return v1.binaryOperation(v2, op);
}

const char * SQLOperationExpression::shortName() const
{
    return "Operation";
}

std::string SQLOperationExpression::asString() const
{
    std::string s(1, op);
    return std::string(shortName()) + "(" + expr1->asString() + " " + s + " " +
	expr2->asString() + ")";
}

SQLValue SQLNotExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);
    // Void or exception should just return.
    if (v.isNull() || v.isException())
	return v;

    return v.asBoolean() ? SQLFalseValue : SQLTrueValue;
}

const char * SQLNotExpression::shortName() const
{
    return "Not";
}

SQLValue SQLNegateExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);

    return v.unaryOperation('-');
}

const char * SQLNegateExpression::shortName() const
{
    return "Negate";
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
    if (v1.isException() || v1.isNull())
	return v1;

    bool got_null = false;

    for (int i = 0; i < list->numExpressions(); i++)
    {
	SQLExpression *e = list->expressionNumber(i);

	SQLValue v2 = e->evaluate(context);
	if (v2.isException())
	    return v2;

        if (v2.isNull())
        {
            got_null = true;
            continue;
        }

	// Convert v2 to the same type as v1
	if (!v2.typeConvert(v1))
	    return SQLValue(new SQLExceptionValue(
                            "Mismatched types in list expression:" +
                            v1.asString() + " and " + v2.asString()));
	if (v1.compare(v2) == 0)
	    return SQLTrueValue;
    }

    if (got_null)
        return SQLValue();
    else
        return SQLFalseValue;
}

// Show the parse tree as a string. This is useful for debugging
std::string SQLInExpression::asString() const
{
    return std::string(shortName()) + "(" + expr->asString() + ", " +
	list->asString() + ")";
}

const char * SQLInExpression::shortName() const
{
    return "In";
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
    return std::string(shortName()) + "(" + expr->asString() + ", <regexp>)";
}

const char * SQLLikeExpression::shortName() const
{
    return "Like";
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

    return std::string(shortName()) + "(" + name + ", " + list->asString() + ")";
}

const char * SQLFunctionExpression::shortName() const
{
    return "Function";
}

SQLValue SQLNullExpression::evaluate(SQLContext &context)
{
    SQLValue v = expr->evaluate(context);
    // Exception should just return.
    if (v.isException())
	return v;

    return v.isNull() ? SQLTrueValue : SQLFalseValue;
}

const char * SQLNullExpression::shortName() const
{
    return "Null";
}

SQLValue SQLVariableExpression::evaluate(SQLContext &context)
{
    return context.variableLookup(className, memberName);
}

const char * SQLVariableExpression::shortName() const
{
    return "Variable";
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

// Extension to allow caching the type conversions
SQLValue SQLValueExpression::evaluateAsType(const SQLValue &v2)
{
    if (value.isSameType(v2))
        return value;

    if (typedValue.isSameType(v2))
        return typedValue;

    // FIXME Messy
    std::string s = value.asString();
    typedValue = SQLValue(new SQLStringValue(s));
    if (typedValue.typeConvert(v2))
        return typedValue;
    else
        return value;
}

const char * SQLValueExpression::shortName() const
{
    return "Value";
}

std::string SQLValueExpression::asString() const
{
    return value.asString();
}
