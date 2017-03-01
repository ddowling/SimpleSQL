/* $Id$
 *
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : constraint_test
 *
 * Author      : Denis Dowling
 * Created     : 25/8/2016
 *
 * Description : Test using the library to express some user input constraints
 */
#include "SQLParse.h"
#include "SQLExpression.h"
#include "SQLContext.h"

#include <iostream>

using namespace std;

// Define the lookup context
class ConstraintContext : public SQLContext
{
public:
    virtual SQLValue variableLookup(const string &class_name,
				    const string &member_name) const;

    int width;
    int height;
};

SQLValue ConstraintContext::variableLookup(const string &class_name,
				           const string &member_name) const
{
    if (member_name == "width")
	return new SQLIntegerValue(width);
    else if (member_name == "height")
	return new SQLIntegerValue(height);
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

int run_query(string s, int test_number)
{
    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the query '" << s << "' : " << endl;
	cerr << parser.errorString() << endl;
    }

    SQLExpression *e = parser.expression();
    ConstraintContext cc;
    if (test_number == 0)
    {
        cc.width = 640;
        cc.height = 480;
    }
    else if (test_number == 1)
    {
        cc.width = 1280;
        cc.height = 960;
    }
    else if (test_number == 2)
    {
        cc.width = 1;
        cc.height = 2;
    }
    else if (test_number == 3)
    {
        cc.width = 640;
        cc.height = 640;
    }
    else if (test_number == 4)
    {
        cc.width = 1280;
        cc.height = 480;
    }
    cout << "Context width=" << cc.width << " height=" << cc.height << endl;

    SQLValue v = e->evaluate(cc);
    if (v.isException())
    {
        cout << "query '" << s << "' generated an exception '"
             << v.asString() << endl;
    }
    else
    {
        cout << "Constraint '" << s << "' evaluates to " << v.asBoolean()
             << endl;
    }
}

int main()
{
    for (int test = 0; test < 5; test++)
    {
	run_query("width in (640, 960, 1280)", test);
	run_query("height in (480, 960, 1280)", test);
	run_query("width = 640 implies height = 480", test);
	run_query("width = 1280 implies height = 960", test);
    }

    return 0;
}
