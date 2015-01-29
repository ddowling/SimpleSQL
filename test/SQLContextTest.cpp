/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLContextTest.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test the functioning of the context
 */
#include "SQLContext.h"
#include <iostream>
#include <assert.h>

using namespace std;

// Define the lookup context
class TestContext
: public SQLContext
{
public:
    TestContext(string class_name, int value);

    virtual SQLValue variableLookup(string class_name,
				    string member_name) const;
private:
    string className;
    int value;
};

TestContext::TestContext(string class_name, int value_)
: className(class_name), value(value_)
{
}

SQLValue TestContext::variableLookup(string class_name,
				     string member_name) const
{
    if (class_name == className)
    {
	if (member_name == "class_name")
	    return new SQLStringValue(className);
	else if (member_name == "value")
	    return new SQLIntegerValue(value);
    }

    // If no match then pass evaluation onto other context if any
    return SQLContext::variableLookup(class_name, member_name);
}

int main()
{
    TestContext ct1("ContextA", 1);

    assert(ct1.getNextInChain() == 0);

    TestContext ct2("ContextB", 2);

    assert(ct2.getNextInChain() == 0);

    // Chain the contexts together.
    ct1.chain(&ct2);

    assert(ct1.getNextInChain() == &ct2);

    // Evaluate some variable lookups of the primary context.
    SQLValue v = ct1.variableLookup("ContextA", "class_name");
    cout << "Lookup got " << v.asString() << endl;
    assert(v.asString() == "ContextA");
    v = ct1.variableLookup("ContextA", "value");
    cout << "Lookup got " << v.asString() << endl;
    assert(v.asInteger() == 1);

    v = ct1.variableLookup("ContextB", "class_name");
    cout << "Lookup got " << v.asString() << endl;
    assert(v.asString() == "ContextB");
    v = ct1.variableLookup("ContextB", "value");
    cout << "Lookup got " << v.asString() << endl;
    assert(v.asInteger() == 2);

    return 0;
}
