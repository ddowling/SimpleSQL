/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : function_test
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test all of the SQL operations
 */
#include <math.h>
#include "SQLParse.h"
#include "SQLExpression.h"
#include "SQLContext.h"

#include <iostream>

using namespace std;

// Define the lookup context
class TaskContext
: public SQLContext
{
public:
    virtual SQLValue variableLookup(string class_name,
				    string member_name) const;
};

SQLValue TaskContext::variableLookup(string class_name,
				     string member_name) const
{
    if (member_name == "x")
	return new SQLIntegerValue(3);
    else if (member_name == "y")
	return new SQLIntegerValue(4);
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

static int total_errors = 0;

void
run_expression(string s, double v2)
{
    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the expression '" << s << "' : "
	     << endl;

	for (int i = 0; i < parser.numErrors(); i++)
	{
	    const SQLParseError *e = parser.errorNumber(i);

	    cout << e->code() << endl;
	}
	total_errors++;
	return ;
    }

    SQLExpression *e = parser.expression();

    TaskContext sc;
    SQLValue val = e->evaluate(sc);

    cout << "expression '" << s << "' evaluated to '" << val.asString()
	 << "'" << endl;

    double v1 = val.asReal();
    if (fabs(v1 - v2) > 0.0001)
    {
	cout << "Error should have been '" << v2 << "'" << endl;
	total_errors++;
    }
}

int main()
{
    // Function Tests
    run_expression("abs(2)", 2);
    run_expression("abs(-2)", 2);
    run_expression("sqrt(4)", 2);
    run_expression("sqrt(9)", 3);
    run_expression("sqrt(2)", sqrt(2));
    run_expression("atan(0.9)", atan(0.9));
    run_expression("cos(3.1415)", cos(3.1415));
    run_expression("cos(3.0)", cos(3.0));
    run_expression("exp(3.0)", exp(3.0));
    run_expression("log(3.0)", log(3.0));
    run_expression("sin(3.0)", sin(3.0));

    run_expression("sqrt(x*x + y*y)", 5);

    // Test some unknown functions
    run_expression("unknown_function(1, 2, 4)", 0);

    return total_errors;
}
