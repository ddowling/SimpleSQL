/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLValueTest.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test the value class and the type conversion operators
 */
#include "SQLValue.h"
#include <iostream>
#include <assert.h>

using namespace std;

int main()
{
    // NULL TESTS
    SQLValue void1(new SQLNullValue());
    SQLValue void2;

    assert(void1 != void2);
    assert(void2 != void2);
    assert(void1 != void1);
    assert(void1.isSameType(void2));
    assert(void2.isSameType(void1));

    //
    // BOOLEAN TESTS
    //
    SQLValue v1(new SQLBooleanValue(true));

    cout << "v1 is " << v1.asString() << endl;

    assert(v1.asBoolean() == true);
    assert(v1.asString() == "True");

    SQLValue v2(new SQLBooleanValue(false));

    cout << "v2 is " << v2.asString() << endl;

    assert(v2.asBoolean() == false);
    assert(v2.asString() == "False");

    SQLValue v3(v1);

    cout << "v3 is " << v3.asString() << endl;

    assert(v3.asBoolean() == true);
    assert(v3.asString() == "True");

    SQLValue v4 = v2;

    cout << "v4 is " << v4.asString() << endl;

    assert(v4.asBoolean() == false);
    assert(v4.asString() == "False");

    // Check that they are all the same type.
    assert(v1.isSameType(v2));
    assert(v2.isSameType(v3));
    assert(v3.isSameType(v4));
    assert(v4.isSameType(v1));

    // Check the compare operator on booleans
    assert(v1.compare(v3) == 0);
    assert(v1 == v3);
    assert(v1.compare(v2) > 0);
    assert(v1 != v2);
    assert(v2.compare(v3) < 0);
    assert(v2 != v3);

    //
    // INTEGER TESTS
    //
    SQLValue v5(new SQLIntegerValue(10));

    cout << "v5 is " << v5.asString() << endl;

    assert(v5.asBoolean() == true);
    assert(v5.asString() == "10");

    SQLValue v6(new SQLIntegerValue(0));

    assert(v6.asBoolean() == false);
    assert(v6.asString() == "0");

    cout << "v6 is " << v6.asString() << endl;

    assert(v5.isSameType(v6));

    // Integers are not the same type as booleans
    assert(!v5.isSameType(v1));

    SQLValue v7(new SQLIntegerValue(3));

    cout << "v7 is " << v7.asString() << endl;

    assert(v7.fromString("10"));

    cout << "v7 is " << v7.asString() << endl;

    assert(v7.asString() == "10");

    // Check the compare operator on integers
    assert(v5.compare(v7) == 0);
    assert(v5.compare(v6) > 0);
    assert(v6.compare(v5) < 0);

    //
    // STRING TESTS
    //
    SQLValue v8(new SQLStringValue("Hello"));

    cout << "v8 is " << v8.asString() << endl;

    assert(v8.asString() == "Hello");

    assert(!v8.isSameType(v1));
    assert(!v8.isSameType(v5));

    SQLValue v9(new SQLStringValue("True"));

    cout << "v9 is " << v9.asString() << endl;

    assert(v9.asString() == "True");
    assert(v9.asBoolean() == true);

    SQLValue v10(new SQLStringValue("False"));

    cout << "v10 is " << v10.asString() << endl;

    assert(v10.asString() == "False");
    assert(v10.asBoolean() == false);

    // Now do some inter type compares
    v9.typeConvert(v1);
    assert(v1.compare(v9) == 0);
    v9.typeConvert(v2);
    assert(v2.compare(v9) < 0);

    v10.typeConvert(v1);
    assert(v1.compare(v10) > 0);
    v10.typeConvert(v2);
    assert(v2.compare(v10) == 0);

    SQLValue v11(new SQLStringValue("10"));

    cout << "v11 is " << v11.asString() << endl;

    assert(v11.asString() == "10");
    assert(v11.asBoolean() == true);

    v11.typeConvert(v5);
    assert(v5.compare(v11) == 0);

#if WXDATE
    // DATETIME TESTS
    wxDateTime today;
    today.ParseDateTime("04/24/1998 14:48:40");
    SQLValue v12(new SQLDateTimeValue(today));

    cout << "v12 is " << v12.asString() << endl;

    assert(v12.asString() == today.Format());

    assert(!v12.isSameType(v1));
    assert(!v12.isSameType(v5));
    assert(!v12.isSameType(v8));

    wxDateTime tomorrow = today + wxTimeSpan::Days(1);
    SQLValue v13(new SQLDateTimeValue(tomorrow));

    // Now do some inter type compares
    assert(v12.compare(v12) == 0);
    v13.typeConvert(v12);
    assert(v12.compare(v13) < 0);

    assert(v13.compare(v12) > 0);
#endif

    //
    // Real TESTS
    //
    SQLValue v15(new SQLRealValue(10.33));

    cout << "v15 is " << v15.asString() << endl;

    assert(v15.asString() == "10.33");

    SQLValue v16(new SQLRealValue(10.22));

    cout << "v16 is " << v16.asString() << endl;

    assert(v16.asString() == "10.22");

    assert(v15.isSameType(v16));

    // Reals are not the same type as booleans
    assert(!v15.isSameType(v1));

    SQLValue v17(new SQLRealValue(10.44));

    cout << "v17 is " << v17.asString() << endl;

    assert(v17.fromString("10.33"));

    cout << "v17 is " << v17.asString() << endl;

    assert(v17.asString() == "10.33");

    // Check the compare operator on Reals
    assert(v15.compare(v17) == 0);
    assert(v15.compare(v16) > 0);
    assert(v16.compare(v15) < 0);

    // Test the exception class
    SQLValue v18(new SQLExceptionValue("Test error"));
    cout << "v18 is " << v18.asString() << endl;

    return 0;
}
