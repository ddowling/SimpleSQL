/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : operation_test.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test all of the SQL operations
 */
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

SQLValue TaskContext::variableLookup(string class_name, string member_name) const
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

void run_expression(string s, SQLValue expected_value)
{
    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the expression '" << s << "' : " << endl;
	for (int i = 0; i < parser.numErrors(); i++)
	{
	    SQLParseError *e = parser.errorNumber(i);

	    cout << e->code() << endl;
	}
	total_errors++;
	return ;
    }

    SQLExpression *e = parser.expression();

    TaskContext sc;
    SQLValue val = e->evaluate(sc);

    cout << "expression '" << s << "' evaluated to '" << val.asString() << "'"
	 << endl;

    if (val != expected_value)
    {
	cout << "Error should have been '" << expected_value.asString()
	     << "'" << endl;
	total_errors++;
    }
}

int main()
{
    // Integer Tests
    run_expression("1 + 1", new SQLIntegerValue(2));
    run_expression("123 + 456", new SQLIntegerValue(579));
    run_expression("123 - 456", new SQLIntegerValue(-333));
    run_expression("456 * 765", new SQLIntegerValue(348840));
    run_expression("50 / 2", new SQLIntegerValue(25));
    run_expression("-(50 + 2)", new SQLIntegerValue(-52));
    run_expression("-(50.2 + 0.8)", new SQLRealValue(-51.0));

    // String tests
    run_expression("'1' + '1'", new SQLStringValue("11"));
    run_expression("'Tom' + 'Dick' + ' Harry'",
	      new SQLStringValue("TomDick Harry"));

    run_expression("x*x + y*y", new SQLIntegerValue(25));

    return total_errors;
}
