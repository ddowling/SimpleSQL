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
#include "SQLContext.h"
#include <iostream>
#include <assert.h>

using namespace std;

class CatchDependanciesContext : public SQLContext
{
public:
    virtual SQLValue variableLookup(const std::string &class_name,
				    const std::string &member_name) const;

};

SQLValue CatchDependanciesContext::variableLookup(const std::string &class_name,
						  const std::string &member_name) const
{
    cout << "got " << class_name << "." << member_name << endl;
    // Return void

    return SQLValue();;
}

void t(string str, string match, bool err = false)
{
    cout << "Parsing : " << str << endl;

    SQLParse parser;
    bool res = parser.parse(str);

    if (parser.numErrors() > 0)
    {
	cout << "Got the following errors:" << endl;
        int col = parser.errorNumber(0)->column();
        if (col < 1)
            col = 1;
        std::string tag(col-1, '-');
        tag += '^';
	cout << str << endl;
        cout << tag << endl;
        cout << parser.errorString() << endl;
    }

    SQLExpression *e = parser.expression();
    if (e != 0)
    {
	cout << "Parse tree is : " << e->asString() << endl;

	CatchDependanciesContext cdc;
        e->evaluate(cdc);
    }

    if (!err)
    {
	assert(res);

        if (!str.empty())
        {
            assert(e != 0);

            if (e->asString() != match)
            {
                cout << "Should be     : " << match << endl;
                cout << endl;
            
                assert(e->asString() == match);
            }
        }
    }
    else
    {
	assert(!res);
    }
}

int main()
{
    t("a", "a");

    t("b", "b");

    t("'string'", "string");
    t("12", "12");

    t("(c)", "c");

    t("a = b", "Equals(a, b)");
    t("a == b", "Equals(a, b)");
    t("a != b", "NotEquals(a, b)");
    t("a < b", "LessThan(a, b)");
    t("a > b", "GreaterThan(a, b)");
    t("a <= b", "LessEquals(a, b)");
    t("a >= b", "GreaterEquals(a, b)");
    t("a and b", "And(a, b)");
    t("a && b", "And(a, b)");
    t("a or b", "Or(a, b)");
    t("a || b", "Or(a, b)");
    t("not a", "Not(a)");
    t("!a", "Not(a)");
    t("a + b", "Operation(a + b)");
    t("a - b", "Operation(a - b)");
    t("a * b", "Operation(a * b)");
    t("a / b", "Operation(a / b)");
    t("-a", "Negate(a)");
    t("f(a, b, c, d)", "Function(f, {a, b, c, d})");
    t("ip.addr = '192.168.44.1' and ip.port in (80, 443) and http.host = 'www.example.com'",
      "And(And(Equals(ip.addr, 192.168.44.1), In(ip.port, {80, 443})), Equals(http.host, www.example.com))");

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

    // Booleans
    t("true", "True");
    t("false", "False");
    t("TRUE", "True");
    t("FALSE", "False");
    t("xx == false", "Equals(xx, False)");

    // Integers
    t("0", "0");
    t("1234", "1234");
    t("-4321", "-4321");

    // Doubles
    t("3.14159", "3.14159");
    t("-47.539", "-47.539");

    // Strings
    t("'string 1'", "string 1");
    t("\"Double quoted string\"", "Double quoted string");
    t("'don\\'t'", "don't");
    t("\"Will also escape this \\\"word\\\".\"",
      "Will also escape this \"word\".");
    t("''", "");
    t("\"\"", "");
    t("'\\n\\t\\r'", "\n\t\r");

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

    cout << "All done" << endl;
    return 0;
}
