/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : query_test.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test the IP address related queries
 */
#include "SQLParse.h"
#include "SQLExpression.h"
#include "SQLContext.h"

#include <iostream>

using namespace std;

#if SQL_IP_SUPPORT

// Define the lookup context
class IPContext : public SQLContext
{
public:
    virtual SQLValue variableLookup(const string &class_name,
                                    const string &member_name) const;
};

SQLValue IPContext::variableLookup(const string &class_name,
                                   const string &member_name) const
{
    if (class_name == "src")
    {
        if (member_name == "addr")
            return new SQLIPAddressValue("192.168.0.1");
        else if (member_name == "port")
            return new SQLIntegerValue(4321);
    }
    else if (class_name == "dest")
    {
        if (member_name == "addr")
            return new SQLIPAddressValue("10.0.0.10");
        else if (member_name == "port")
            return new SQLIntegerValue(80);
    }
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

static int total_errors = 0;

void run_query(const string &s,
               bool expected_result,
               bool expect_exception = false)
{
    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the query '" << s << "' : " << endl;
	cerr << parser.errorString() << endl;
	total_errors++;
    }

    SQLExpression *e = parser.expression();

    IPContext sc;

    SQLValue v = e->evaluate(sc);

    if (v.isException())
    {
        cout << "query '" << s << "' generated an exception '"
             << v.asString() << endl;
        if (!expect_exception)
        {
            cout << "Did not expect this exception" << endl;
            total_errors++;
        }
    }
    else if (expect_exception)
    {
        cout << "Was expecting an exception but did not get one" << endl;
        total_errors++;
    }

    bool result = e->evaluate(sc).asBoolean();

    cout << "query '" << s << "' evaluated to " << result << endl;

    if (result != expected_result)
    {
	cout << "Did not get the expected result " << expected_result
	     << endl;
	total_errors++;
    }
}

int main()
{
    run_query("src.addr == 192.168.0.1", true);
    run_query("src.addr > 192.168.0.0", true);
    run_query("src.addr > 10.255.255.255", true);
    run_query("src.addr < 192.168.0.2", true);
    run_query("src.addr < 192.168.10.0", true);
    run_query("'4.3.2.1' == 4.3.2.1", true);
    run_query("src.addr between 192.168.0.0 and 192.168.255.255", true);
    run_query("src.addr between 10.0.0.0 and 10.255.255.255", false);
    run_query("src.addr not between 192.168.0.0 and 192.168.255.255", false);
    run_query("src.addr not between 10.0.0.0 and 10.255.255.255", true);
    run_query("src.addr == 192.168.0.1 and dest.port == 80", true);

    cout << "Found a total of " << total_errors << " errors" << endl;

    return total_errors;
}
#else

int main()
{
    cout << "Skipping tests as no IP support" << endl;

    return 0;
}

#endif
