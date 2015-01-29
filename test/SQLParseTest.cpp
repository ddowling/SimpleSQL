/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLParseTest.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test the SQL Parser.
 */

#include "SQLParse.h"
#include "SQLExpression.h"
#include <iostream>
#include <assert.h>

using namespace std;

void
t(string str, string match, bool err = false)
{
    SQLParse parser;

    bool res = parser.parse(str);

    if (!res)
    {
	cout << str << endl;
	cout << "Got the following errors:" << endl;

	for(int i = 0; i < parser.numErrors(); i++)
	{
	    SQLParseError *err = parser.errorNumber(i);
	    cout << "Error : " << err->code()
		 << " Index of " << err->index() << endl;
	}
    }

    SQLExpression *e = parser.expression();
    if (e != 0)
	cout << "Parse tree is : " << e->asString() << endl;

    if (!err)
    {
	assert(res);

	if (!match.empty())
	{
	    assert(e != 0);

	    if (e->asString() != match)
	    {
		cout << "Should be     : " << match << endl;
		cout << endl;

		assert(e->asString() == match);
	    }
	}
	else
	{
	    // Null expression if empty string.
	    assert(e == 0);
	}
    }
    else
	assert(!res);
}

int main()
{
    t("a", "a");

    t("b", "b");

    t("'string'", "string");
    t("12", "12");

    t("(c)", "c");

    t("a = b", "Equals(a, b)");
    t("a != b", "NotEquals(a, b)");
    t("a < b", "LessThan(a, b)");
    t("a > b", "GreaterThan(a, b)");
    t("a <= b", "LessEquals(a, b)");
    t("a >= b", "GreaterEquals(a, b)");
    t("a and b", "And(a, b)");
    t("a or b", "Or(a, b)");
    t("not a", "Not(a)");
    t("a + b", "Operation(a + b)");
    t("a - b", "Operation(a - b)");
    t("a * b", "Operation(a * b)");
    t("a / b", "Operation(a / b)");
    t("-a", "Negate(a)");
    t("f(a, b, c, d)", "Function(f, {a, b, c, d})");

    // Parse an empty expression
    t("", "");

    // Now some more compilcated examples
    t("(a != b) and (a != c)",
      "And(NotEquals(a, b), NotEquals(a, c))");
    t("a and b and c",
      "And(And(a, b), c)");

    // Check the operator ordering
    t("a < b and c < d",
      "And(LessThan(a, b), LessThan(c, d))");
    t("a and b < c and d",
      "And(And(a, LessThan(b, c)), d)");

    t("not a and b",
      "And(Not(a), b)");

    t("a and not b",
      "And(a, Not(b))");

    // Check the in operator
    t("a in ('Tom', 'Dick' )",
      "In(a, {Tom, Dick})");
    t("a not in ('Tom', 'Dick' )",
      "Not(In(a, {Tom, Dick}))");

    // Check the between operator
    t("a between b and c",
      "And(GreaterEquals(a, b), LessEquals(a, c))");
    t("a not between b and c",
      "Or(LessThan(a, b), GreaterThan(a, c))");

    // Now add some syntax errors
    t("a b", "", true);
    t("( b c )", "", true);
    t("a <", "", true);
    t("(a < ) and b", "", true);
    t("( b ", "", true);
    t("b)", "", true);
    t("a < > b", "", true);
    t("a != 'fred", "", true);
    t("a = 'xxx", "", true);
    return 0;
}
