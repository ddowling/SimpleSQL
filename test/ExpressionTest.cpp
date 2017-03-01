/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : ExpressionTest.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Test the expression classes.
 */
#include "SQLExpression.h"
#include "SQLContext.h"
#include <iostream>
#include <assert.h>

using namespace std;

class TestContext
: public SQLContext
{
public:
    virtual SQLValue variableLookup(const string &class_name,
				    const string &member_name) const;
};

SQLValue TestContext::variableLookup(const string &class_name,
                                     const string &member_name) const
{
    if (member_name == "a")
	return SQLValue(new SQLStringValue("Tom"));
    else if (member_name == "b")
	return SQLValue(new SQLStringValue("Dick"));
    else if (member_name == "c")
	return SQLValue(new SQLStringValue("Harry"));
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

// Destroy the expression tree.
void f(SQLExpression *e)
{
    e->getRef();
    e->releaseRef();
}

int main()
{
    TestContext c;

    SQLValueExpression *te =
	new SQLValueExpression(SQLExpression::SQLTrueValue);
    SQLValueExpression *fe =
	new SQLValueExpression(SQLExpression::SQLFalseValue);
    SQLValueExpression *ve =
        new SQLValueExpression(SQLValue());

    // Get an extra ref on these values to prevent them from being deleted
    te->getRef();
    fe->getRef();
    ve->getRef();

    cout << "Equals test" << endl;
    SQLEqualsExpression *e1 = new SQLEqualsExpression(te, te);
    cout << e1->asString() << " gives " << e1->evaluate(c).asString() << endl;
    assert(e1->evaluate(c).asBoolean() == true);
    f(e1);

    SQLEqualsExpression *e2 = new SQLEqualsExpression(te, fe);
    cout << e2->asString() << " gives " << e2->evaluate(c).asString() << endl;
    assert(e2->evaluate(c).asBoolean() == false);
    f(e2);

    SQLEqualsExpression *e3 = new SQLEqualsExpression(fe, fe);
    cout << e3->asString() << " gives " << e3->evaluate(c).asString() << endl;
    assert(e3->evaluate(c).asBoolean() == true);
    f(e3);

    SQLEqualsExpression *e4 = new SQLEqualsExpression(fe, te);
    cout << e4->asString() << " gives " << e4->evaluate(c).asString() << endl;
    assert(e4->evaluate(c).asBoolean() == false);
    f(e4);

    cout << "NotEquals test" << endl;
    SQLNotEqualsExpression *ne1 = new SQLNotEqualsExpression(te, te);
    cout << ne1->asString() << " gives " << ne1->evaluate(c).asString()
	 << endl;
    assert(ne1->evaluate(c).asBoolean() == false);
    f(ne1);

    SQLNotEqualsExpression *ne2 = new SQLNotEqualsExpression(te, fe);
    cout << ne2->asString() << " gives " << ne2->evaluate(c).asString()
	 << endl;
    assert(ne2->evaluate(c).asBoolean() == true);
    f(ne2);

    SQLNotEqualsExpression *ne3 = new SQLNotEqualsExpression(fe, fe);
    cout << ne3->asString() << " gives " << ne3->evaluate(c).asString()
	 << endl;
    assert(ne3->evaluate(c).asBoolean() == false);
    f(ne3);

    SQLNotEqualsExpression *ne4 = new SQLNotEqualsExpression(fe, te);
    cout << ne4->asString() << " gives " << ne4->evaluate(c).asString()
	 << endl;
    assert(ne4->evaluate(c).asBoolean() == true);
    f(ne4);

    cout << "GreaterThan test" << endl;

    SQLGreaterThanExpression *gt1 = new SQLGreaterThanExpression(te, te);
    cout << gt1->asString() << " gives " << gt1->evaluate(c).asString()
	 << endl;
    assert(gt1->evaluate(c).asBoolean() == false);
    f(gt1);

    SQLGreaterThanExpression *gt2 = new SQLGreaterThanExpression(te, fe);
    cout << gt2->asString() << " gives " << gt2->evaluate(c).asString()
	 << endl;
    assert(gt2->evaluate(c).asBoolean() == true);
    f(gt2);

    SQLGreaterThanExpression *gt3 = new SQLGreaterThanExpression(fe, fe);
    cout << gt3->asString() << " gives " << gt3->evaluate(c).asString()
	 << endl;
    assert(gt3->evaluate(c).asBoolean() == false);
    f(gt3);

    SQLGreaterThanExpression *gt4 = new SQLGreaterThanExpression(fe, te);
    cout << gt4->asString() << " gives " << gt4->evaluate(c).asString()
	 << endl;
    assert(gt4->evaluate(c).asBoolean() == false);
    f(gt4);

    cout << "LessThan test" << endl;
    SQLLessThanExpression *lt1 = new SQLLessThanExpression(te, te);
    cout << lt1->asString() << " gives " << lt1->evaluate(c).asString()
	 << endl;
    assert(lt1->evaluate(c).asBoolean() == false);
    f(lt1);

    SQLLessThanExpression *lt2 = new SQLLessThanExpression(te, fe);
    cout << lt2->asString() << " gives " << lt2->evaluate(c).asString()
	 << endl;
    assert(lt2->evaluate(c).asBoolean() == false);
    f(lt2);

    SQLLessThanExpression *lt3 = new SQLLessThanExpression(fe, fe);
    cout << lt3->asString() << " gives " << lt3->evaluate(c).asString()
	 << endl;
    assert(lt3->evaluate(c).asBoolean() == false);
    f(lt3);

    SQLLessThanExpression *lt4 = new SQLLessThanExpression(fe, te);
    cout << lt4->asString() << " gives " << lt4->evaluate(c).asString()
	 << endl;
    assert(lt4->evaluate(c).asBoolean() == true);
    f(lt4);

    cout << "GreaterEqual test" << endl;
    SQLGreaterEqualsExpression *ge1 = new SQLGreaterEqualsExpression(te, te);
    cout << ge1->asString() << " gives " << ge1->evaluate(c).asString()
	 << endl;
    assert(ge1->evaluate(c).asBoolean() == true);
    f(ge1);

    SQLGreaterEqualsExpression *ge2 = new SQLGreaterEqualsExpression(te, fe);
    cout << ge2->asString() << " gives " << ge2->evaluate(c).asString()
	 << endl;
    assert(ge2->evaluate(c).asBoolean() == true);
    f(ge2);

    SQLGreaterEqualsExpression *ge3 = new SQLGreaterEqualsExpression(fe, fe);
    cout << ge3->asString() << " gives " << ge3->evaluate(c).asString()
	 << endl;
    assert(ge3->evaluate(c).asBoolean() == true);
    f(ge3);

    SQLGreaterEqualsExpression *ge4 = new SQLGreaterEqualsExpression(fe, te);
    cout << ge4->asString() << " gives " << ge4->evaluate(c).asString()
	 << endl;
    assert(ge4->evaluate(c).asBoolean() == false);
    f(ge4);

    cout << "LessEquals test" << endl;
    SQLLessEqualsExpression *le1 = new SQLLessEqualsExpression(te, te);
    cout << le1->asString() << " gives " << le1->evaluate(c).asString()
	 << endl;
    assert(le1->evaluate(c).asBoolean() == true);
    f(le1);

    SQLLessEqualsExpression *le2 = new SQLLessEqualsExpression(te, fe);
    cout << le2->asString() << " gives " << le2->evaluate(c).asString()
	 << endl;
    assert(le2->evaluate(c).asBoolean() == false);
    f(le2);

    SQLLessEqualsExpression *le3 = new SQLLessEqualsExpression(fe, fe);
    cout << le3->asString() << " gives " << le3->evaluate(c).asString()
	 << endl;
    assert(le3->evaluate(c).asBoolean() == true);
    f(le3);

    SQLLessEqualsExpression *le4 = new SQLLessEqualsExpression(fe, te);
    cout << le4->asString() << " gives " << le4->evaluate(c).asString()
	 << endl;
    assert(le4->evaluate(c).asBoolean() == true);
    f(le4);

    cout << "And test" << endl;
    SQLAndExpression *a1 = new SQLAndExpression(te, te);
    cout << a1->asString() << " gives " << a1->evaluate(c).asString() << endl;
    assert(a1->evaluate(c).asBoolean() == true);
    f(a1);

    SQLAndExpression *a2 = new SQLAndExpression(te, fe);
    cout << a2->asString() << " gives " << a2->evaluate(c).asString() << endl;
    assert(a2->evaluate(c).asBoolean() == false);
    f(a2);

    SQLAndExpression *a3 = new SQLAndExpression(fe, fe);
    cout << a3->asString() << " gives " << a3->evaluate(c).asString() << endl;
    assert(a3->evaluate(c).asBoolean() == false);
    f(a3);

    SQLAndExpression *a4 = new SQLAndExpression(fe, te);
    cout << a4->asString() << " gives " << a4->evaluate(c).asString() << endl;
    assert(a4->evaluate(c).asBoolean() == false);
    f(a4);

    SQLAndExpression *a5 = new SQLAndExpression(fe, ve);
    cout << a5->asString() << " gives " << a5->evaluate(c).asString() << endl;
    assert(a5->evaluate(c).asBoolean() == false);
    assert(!a5->evaluate(c).isVoid());
    f(a5);

    SQLAndExpression *a6 = new SQLAndExpression(te, ve);
    cout << a6->asString() << " gives " << a6->evaluate(c).asString() << endl;
    assert(a6->evaluate(c).asBoolean() == false);
    assert(a6->evaluate(c).isVoid());
    f(a6);

    SQLAndExpression *a7 = new SQLAndExpression(ve, fe);
    cout << a7->asString() << " gives " << a7->evaluate(c).asString() << endl;
    assert(a7->evaluate(c).asBoolean() == false);
    assert(!a7->evaluate(c).isVoid());
    f(a7);

    SQLAndExpression *a8 = new SQLAndExpression(ve, te);
    cout << a8->asString() << " gives " << a8->evaluate(c).asString() << endl;
    assert(a8->evaluate(c).asBoolean() == false);
    assert(a8->evaluate(c).isVoid());
    f(a8);

    cout << "Or test" << endl;
    SQLOrExpression *o1 = new SQLOrExpression(te, te);
    cout << o1->asString() << " gives " << o1->evaluate(c).asString() << endl;
    assert(o1->evaluate(c).asBoolean() == true);
    f(o1);

    SQLOrExpression *o2 = new SQLOrExpression(te, fe);
    cout << o2->asString() << " gives " << o2->evaluate(c).asString() << endl;
    assert(o2->evaluate(c).asBoolean() == true);
    f(o2);

    SQLOrExpression *o3 = new SQLOrExpression(fe, fe);
    cout << o3->asString() << " gives " << o3->evaluate(c).asString() << endl;
    assert(o3->evaluate(c).asBoolean() == false);
    f(o3);

    SQLOrExpression *o4 = new SQLOrExpression(fe, te);
    cout << o4->asString() << " gives " << o4->evaluate(c).asString() << endl;
    assert(o4->evaluate(c).asBoolean() == true);
    f(o4);

    SQLOrExpression *o5 = new SQLOrExpression(fe, ve);
    cout << o5->asString() << " gives " << o5->evaluate(c).asString() << endl;
    assert(o5->evaluate(c).asBoolean() == false);
    assert(o5->evaluate(c).isVoid());
    f(o5);

    SQLOrExpression *o6 = new SQLOrExpression(te, ve);
    cout << o6->asString() << " gives " << o6->evaluate(c).asString() << endl;
    assert(o6->evaluate(c).asBoolean() == true);
    assert(!o6->evaluate(c).isVoid());
    f(o6);

    SQLOrExpression *o7 = new SQLOrExpression(ve, fe);
    cout << o7->asString() << " gives " << o7->evaluate(c).asString() << endl;
    assert(o7->evaluate(c).asBoolean() == false);
    assert(o7->evaluate(c).isVoid());
    f(o7);

    SQLOrExpression *o8 = new SQLOrExpression(ve, te);
    cout << o8->asString() << " gives " << o8->evaluate(c).asString() << endl;
    assert(o8->evaluate(c).asBoolean() == true);
    assert(!o8->evaluate(c).isVoid());
    f(o8);

    cout << "Exclusive or test" << endl;
    SQLXorExpression *x1 = new SQLXorExpression(fe, fe);
    cout << x1->asString() << " gives " << x1->evaluate(c).asString() << endl;
    assert(x1->evaluate(c).asBoolean() == FALSE);
    f(x1);

    SQLXorExpression *x2 = new SQLXorExpression(te, fe);
    cout << x2->asString() << " gives " << x2->evaluate(c).asString() << endl;
    assert(x2->evaluate(c).asBoolean() == TRUE);
    f(x2);

    SQLXorExpression *x3 = new SQLXorExpression(fe, te);
    cout << x3->asString() << " gives " << x3->evaluate(c).asString() << endl;
    assert(x3->evaluate(c).asBoolean() == TRUE);
    f(x3);

    SQLXorExpression *x4 = new SQLXorExpression(te, te);
    cout << x4->asString() << " gives " << x4->evaluate(c).asString() << endl;
    assert(x4->evaluate(c).asBoolean() == FALSE);
    f(x4);

    cout << "Not test" << endl;
    SQLNotExpression *not1 = new SQLNotExpression(te);
    cout << not1->asString() << " gives " << not1->evaluate(c).asString()
	 << endl;
    assert(not1->evaluate(c).asBoolean() == false);
    f(not1);

    SQLNotExpression *not2 = new SQLNotExpression(fe);
    cout << not2->asString() << " gives " << not2->evaluate(c).asString()
	 << endl;
    assert(not2->evaluate(c).asBoolean() == true);
    f(not2);

    SQLNotExpression *not3 = new SQLNotExpression(ve);
    cout << not3->asString() << " gives " << not3->evaluate(c).asString()
	 << endl;
    assert(not3->evaluate(c).asBoolean() == false);
    assert(not3->evaluate(c).isVoid());
    f(not3);

    cout << "VariableExpression test" << endl;
    SQLVariableExpression *ve1 = new SQLVariableExpression("", "a");
    cout << ve1->asString() << " gives " << ve1->evaluate(c).asString()
	 << endl;
    assert(ve1->evaluate(c).asString() == "Tom");
    f(ve1);

    SQLVariableExpression *ve2 = new SQLVariableExpression("", "b");
    cout << ve2->asString() << " gives " << ve2->evaluate(c).asString()
	 << endl;
    assert(ve2->evaluate(c).asString() == "Dick");
    f(ve2);

    SQLVariableExpression *ve3 = new SQLVariableExpression("", "c");
    cout << ve3->asString() << " gives " << ve3->evaluate(c).asString()
	 << endl;
    assert(ve3->evaluate(c).asString() == "Harry");
    f(ve3);

    SQLVariableExpression *ve4 = new SQLVariableExpression("", "d");
    SQLValue v = ve4->evaluate(c);
    cout << ve4->asString() << " gives " << v.asString()
	 << endl;
    assert(v.isException());
    assert(v.asString() == "Unknown variable 'd'");
    f(ve4);

    // Set up a somewhat complex expression and destroy it
    SQLExpression *e =
     new SQLOrExpression(new SQLAndExpression(
			     new SQLVariableExpression("", "a"),
			     new SQLVariableExpression("", "b")),
			 new SQLNotExpression(
			     new SQLVariableExpression("", "c")));

    cout << e->asString() << " gives " << e->evaluate(c).asString() << endl;
    assert(e->evaluate(c).asBoolean() == true);

    f(e);

    // remove the extra ref on te and fe to delete the values
    te->releaseRef();
    fe->releaseRef();

    return 0;
}
