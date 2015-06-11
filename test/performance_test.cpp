/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : performance_test.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : The test module tests how fast the evaluate cycle is in the
 *               system.
 */
#include "SQLParse.h"
#include "SQLExpression.h"
#include "SQLContext.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

struct Shift
{
#if SQL_DATE_SUPPORT
    time_t start;
    time_t end;
#endif

    string status;
    string unit;
    string level;
};

const int max_shifts = 100000;

static Shift **shifts;

void make_shifts()
{
    shifts = new Shift *[max_shifts];

#if SQL_DATE_SUPPORT
    SQLDateTimeValue dt;
    dt.fromString("01/12/2010 00:00:00");
#endif

    for(int i = 0; i < max_shifts; i++)
    {
	Shift *s = new Shift;
	shifts[i] = s;

#if SQL_DATE_SUPPORT
	s->start = dt.getValue() + (rand() % 3600*17);
	s->end = s->start + (rand() % 3600*9);
#endif

	int res = rand() % 4;
	switch(res)
	{
	case 0:
	    s->status = "O";
	    break;
	case 1:
	    s->status = "W";
	    break;
	case 2:
	    s->status = "L";
	    break;
	case 3:
	    s->status = "T";
	    break;
	default:
	    assert(0);
	}

	stringstream ss;
	ss << "Unit " << (rand() % 4) + 1;
	s->unit = ss.str();

	res = rand() % 3;
	switch(res)
	{
	case 0:
	    s->level = "ASO";
	    break;
	case 1:
	    s->level = "Supervisor";
	    break;
	case 2:
	    s->level = "Ambassador";
	    break;
	default:
	    assert(0);
	}
    }
}

// Define the lookup context
class ShiftContext
: public SQLContext
{
public:
    virtual SQLValue variableLookup(const string &class_name,
				    const string &member_name) const;
    Shift *shift_;
};

SQLValue ShiftContext::variableLookup(const string &class_name,
				      const string &member_name) const
{
#if SQL_DATE_SUPPORT
    if (member_name == "start")
	return new SQLDateTimeValue(shift_->start);
    else if (member_name == "end")
	return new SQLDateTimeValue(shift_->end);
#endif
    if (member_name == "status")
	return new SQLStringValue(shift_->status);
    else if (member_name == "unit")
	return new SQLStringValue(shift_->unit);
    else if (member_name == "level")
	return new SQLStringValue(shift_->level);
    else
	// If no match then pass evaluation onto other context if any
	return SQLContext::variableLookup(class_name, member_name);
}

double diff(struct timeval &end, struct timeval &start)
{
    double d = end.tv_sec * 1000.0 + (double)end.tv_usec/1.0E3;
    d -=  start.tv_sec * 1000.0 + (double)start.tv_usec/1.0E3;

    return d;
}

int run_query(const string &s)
{
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, 0);

    SQLParse parser;

    if (!parser.parse(s))
    {
	cerr << "Could not parse the query '" << s << "' : " << endl;
	for (int i = 0; i < parser.numErrors(); i++)
	{
	    const SQLParseError *e = parser.errorNumber(i);

	    cout << e->code() << endl;
	}
	return -1;
    }

    SQLExpression *e = parser.expression();

    gettimeofday(&end, 0);

    cout << "Parse took " << diff(end, start) << " milliseconds" << endl;

    gettimeofday(&start, 0);

    int count = 0;

    ShiftContext sc;

    for(int i = 0; i < max_shifts; i++)
    {
	sc.shift_ = shifts[i];

	SQLValue v = e->evaluate(sc);

	if (v.isVoid() || v.isException())
	{
	    cerr << "Query '" << s
		 << "' did not return a boolean result "
		 << v.asString()
		 << endl;
	    break;
	}
	else if (v.asBoolean())
	    count++;
    }

    gettimeofday(&end, 0);

    cout << "Query took " << diff(end, start) << " milliseconds" << endl;

    cout << "Query '" << s << "' match " << count << " records out of "
	 << max_shifts << endl;
    cout << endl;

    return count;
}

#if SQL_DATE_SUPPORT
int run_hard_query1(const string &status,
                    const string &level,
                    const string &unit)
{
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, 0);

    SQLDateTimeValue dt;
    dt.fromString("01/12/2010 12:00:00");
    time_t dt_t = dt.getValue();

    int count = 0;
    for(int i = 0; i < max_shifts; i++)
    {
	Shift *s = shifts[i];

	if (s->status == status && s->start < dt_t && s->end > dt_t
	    && s->level == level && s->unit == unit)
	    count++;
    }

    gettimeofday(&end, 0);

    cout << "Query took " << diff(end, start) << " milliseconds" << endl;

    cout << "Query hard_query1() match " << count << " records out of "
	 << max_shifts << endl;
    cout << endl;

    return count;
}
#endif

int main()
{
#if SQL_DATE_SUPPORT
    // FIXME date time parsing is very rigid
    SQLDateTimeValue::setFormat("%d/%m/%Y %H:%M:%S");
#endif

    cout << "Building Shifts" << endl;

    make_shifts();

    int r1 = run_query("status = 'O'");
    int r2 = run_query("status = 'L'");
    int r3 = run_query("status = 'O' or status = 'L'");

    assert(r1 + r2 == r3);

    int r4 = run_query("status = 'W'");
    int r5 = run_query("status = 'T'");

    assert(r1 + r2 + r4 + r5 == max_shifts);

#if SQL_DATE_SUPPORT
    // Find all the ASOs in unit one who are on duty at 1200.
    string s =
	"status = 'W' and start < '01/12/2010 12:00:00'"
	" and end > '01/12/2010 12:00:00'"
	" and level = 'ASO' and unit = 'Unit 1'";

    int r6 = run_query(s);

    int r7 = run_hard_query1("W", "ASO", "Unit 1");

    assert(r6 == r7);

    s = "status = 'O' and start < '01/12/2010 12:00:00'"
	" and end > '01/12/2010 12:00:00'"
	" and level = 'Supervisor' and unit = 'Unit 3'";

    r6 = run_query(s);

    r7 = run_hard_query1("O", "Supervisor", "Unit 3");

    assert(r6 == r7);
#endif

    return 0;
}
