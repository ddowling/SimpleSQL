/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLContext.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Context pattern to map variables and functions to values
 */
#include "math.h"
#include "SQLContext.h"

// Create a context
SQLContext::SQLContext()
: nextInChain(0)
{
}

SQLContext::~SQLContext()
{
}

// chain another context to be evaluated after this context
// is evaluated
void SQLContext::chain(SQLContext *c)
{
    SQLContext *old_next = nextInChain;

    nextInChain = c;

    if (c != 0)
	c->chain(old_next);
}

SQLContext * SQLContext::getNextInChain() const
{
    return nextInChain;
}

SQLValue SQLContext::variableLookup(const std::string &class_name,
				    const std::string &member_name) const
{
    if (nextInChain != 0)
	return nextInChain->variableLookup(class_name, member_name);
    else
    {
	std::string var;

	if (!class_name.empty())
	    var = class_name + ".";
	var += member_name;

	return SQLValue(new SQLExceptionValue("Unknown variable '" +
					      var + "'"));
    }
}

SQLValue SQLContext::functionLookup(const std::string &class_name,
				    const std::string &member_name,
				    int num_args, SQLValue *args)
{
    if (nextInChain != 0)
	return nextInChain->functionLookup(class_name, member_name,
					    num_args, args);
    else
	return defaultFunctionLookup(class_name, member_name,
				     num_args, args);
}

SQLValue SQLContext::defaultFunctionLookup(const std::string &class_name,
					   const std::string &member_name,
					   int num_args, SQLValue *args)
{
    // These functions should have global scope
    if (class_name.empty())
    {
	// Functions that take one argument
	if (num_args == 1)
	{
	    double arg1 = args[0].asReal();
	    double result;
	    if (member_name == "abs")
		result = fabs(arg1);
	    else if (member_name == "atan")
		result = atan(arg1);
	    else if (member_name == "cos")
		result = cos(arg1);
	    else if (member_name == "exp")
		result = exp(arg1);
	    else if (member_name == "log")
		result = log(arg1);
	    else if (member_name == "sin")
		result = sin(arg1);
	    else if (member_name == "sqrt")
		result = sqrt(arg1);
	    else
		return SQLValue(new SQLExceptionValue(
					   "Unknown function '" +
					   member_name + "'"));

	    return SQLValue(new SQLRealValue(result));
	}


	return SQLValue(new SQLExceptionValue("Unknown function '" +
					      member_name + "'"));
    }

    // Return exception is no match is possible.
    return SQLValue(new SQLExceptionValue("Unknown function '" +
					  class_name + "." + member_name +
					  "'"));
}
