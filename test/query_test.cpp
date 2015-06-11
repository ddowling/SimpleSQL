/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : query_test.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : The test module tests the operation of some more complex queries
 */
#include "SQLParse.h"
#include "SQLExpression.h"
#include "SQLContext.h"

#include <iostream>

using namespace std;

struct Task
{
#if SQL_DATE_SUPPORT
    time_t start;
    time_t end;
#endif
    string status;
    int crews;
    string remark;
};

static int max_tasks = 10;
static Task **tasks;

int total_errors = 0;

void make_tasks()
{
    tasks = new Task *[max_tasks];

#if SQL_DATE_SUPPORT
    SQLDateTimeValue dt;
    dt.fromString("00:00 01/12/2010");
    time_t dt_t = dt.getValue();
#endif

    for(int i = 0; i < max_tasks; i++)
    {
	Task *t = new Task;
	tasks[i] = t;

#if SQL_DATE_SUPPORT
	t->start = dt_t + (i * 3600);
	t->end = dt_t + ((i+1) * 3600);
#endif

	const char *status[4] = { "Driving", "Miscellaneous",
				  "Travelling", "Shunting" };

	t->status = status[i % 4];

	t->crews = i + 1;

	const char *remark[10] = { "Remark1", "Remark2", "Rem3",
				   "remark4", "", "r5", "Remark6",
				   "7remark", "8remark", "" };
	t->remark = remark[i % 10];
    }
}

// Define the lookup context
class TaskContext
: public SQLContext
{
public:
    virtual SQLValue variableLookup(const string &class_name,
                                    const string &member_name) const;
    Task *task_;
};

SQLValue TaskContext::variableLookup(const string &class_name,
				     const string &member_name) const
{
#if SQL_DATE_SUPPORT
    if (member_name == "start")
	return new SQLDateTimeValue(task_->start);
    else if (member_name == "end")
	return new SQLDateTimeValue(task_->end);
#endif
    if (member_name == "status")
	return new SQLStringValue(task_->status);
    else if (member_name == "crews")
	return new SQLIntegerValue(task_->crews);
    else if (member_name == "remark")
    {
	if (task_->remark.empty())
	    return SQLValue(new SQLVoidValue);
	else
	    return new SQLStringValue(task_->remark);
    }
    else if (member_name == "void_value")
	return new SQLVoidValue;
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

int run_query(const string &s,
              int expected_count,
	      bool expect_exception = false,
              bool expect_void = false)
{
    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the query '" << s << "' : " << endl;
	cerr << parser.errorString() << endl;
	total_errors++;
    }

    SQLExpression *e = parser.expression();

    int count = 0;

    TaskContext sc;

    for(int i = 0; i < max_tasks; i++)
    {
	sc.task_ = tasks[i];


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

	    break;
	}
	else if (expect_exception)
	{
	    cout << "Was expecting an exception but did not get one" << endl;
	    total_errors++;
	    break;
	}

	if (v.isVoid() && !expect_void)
        {
            cout << "Did not expect a void value" << endl;
            total_errors++;
        }

	if (e->evaluate(sc).asBoolean())
	    count++;
    }

    cout << "query '" << s << "' matched " << count << " tasks" << endl;

    if (count != expected_count)
    {
	cout << "Did not get the expected match count " << expected_count
	     << endl;
	total_errors++;
    }

    return count;
}

int main()
{
#if SQL_DATE_SUPPORT
    // FIXME date time parsing if very rigid
    SQLDateTimeValue::setFormat("%H:%M %d/%m/%Y");
#endif

    make_tasks();

#if SQL_DATE_SUPPORT
    run_query("start between '05:00 1/12/2010' and '7:00 1/12/2010'",
	      3);
    run_query("end not between '03:00 1/12/2010' and '5:00 1/12/2010'",
	      7);
#endif
    run_query("remark like 'Remark_'", 3, false, true);
    run_query("remark like 'Rem%'", 4, false, true);
    run_query("remark like '%k'", 2, false, true);
    run_query("crews = 10", 1);
    run_query("crews = 0", 0);
    run_query("remark = 'Remark1'", 1, false, true);
    run_query("remark is null", 2);
    run_query("remark in ('Remark1', 'Remark2')", 2, false, true);
    run_query("crews in (1, 2, 3, 4, 5)", 5);
    run_query("crews in ('1', '2', '3', '4', '5')", 5);
    run_query("crews not in ('1', '3', '4', '5')", 6);
    run_query("crews in (4, 5)", 2);
    run_query("crews not in (4, 5)", 8);
    // The escape here turns the expression into a regular expression
    run_query("crews like 'X[1-3]' escape 'X'", 3);
    run_query("crews != 3 and crews != 4", 8);
    run_query("crews = 5 or crews = 7", 2);
    run_query("crews < 5", 4);
    run_query("crews <= 5", 5);
    run_query("crews > 5", 5);
    run_query("crews >= 5", 6);

    // Test some void values that should return no matches and errors
    run_query("xxx >= 5", 0, true);
    run_query("xxx <= 5", 0, true);
    run_query("yyy < 0", 0, true);
    run_query("yyy = 0", 0, true);
    run_query("yyy != 0", 0, true);
    run_query("yyy > 0", 0, true);
    run_query("1 >= xxx", 0, true);
    run_query("2 <= xxx", 0, true);
    run_query("3 < yyy", 0, true);
    run_query("4 = yyy", 0, true);
    run_query("4 != yyy", 0, true);
    run_query("5 > xxx.yyy", 0, true);

    // Test some other types on run time exceptions
    run_query("'abc' = 5", 0, false);
    run_query("5 = 'abc'", 0, true);
    run_query("5 = 5.0", 10, false);
    run_query("7.0 = 7", 10, false);
    run_query("'vvv'", 0);

    // Test some queries containing void values that should run
    // without error.
    run_query("void_value = 'fred'", 0, false, true);
    run_query("void_value = ''", 0, false, true);
    run_query("void_value is null", 10);
    run_query("void_value = 'fred' or crews > 5", 5, false, true);

    run_query("crews > 5 or void_value = 'fred'", 5, false, true);
    run_query("void_value or crews > 5", 5, false, true);
    run_query("crews > 5 or void_value", 5, false, true);

    run_query("void_value = 'fred' and crews > 5", 0, false, true);
    run_query("crews > 5 and void_value = 'fred'", 0, false, true);
    run_query("not (void_value = 'fred') and crews > 5", 0, false, true);
    run_query("crews > 5 and not (void_value = 'fred')", 0, false, true);
    run_query("void_value and crews > 5", 0, false, true);
    run_query("crews > 5 and void_value", 0, false, true);

    cout << "Found a total of " << total_errors << " errors" << endl;

    return total_errors;
}
