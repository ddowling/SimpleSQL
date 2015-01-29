/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLValue.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Variant value class for SQL values
 */
#include "SQLValue.h"
#include <assert.h>
#include <string.h>
#include <sstream>

#if DEBUG_TIME
#include <stdio.h>
#endif

SQLVoidValue * SQLValue::void_rep_ = 0;

SQLValue::SQLValue()
{
    if (void_rep_ == 0)
    {
	void_rep_ = new SQLVoidValue;
	// Bump the reference count so it will not be destroyed.
	void_rep_->refCount_++;
    }

    setRep(void_rep_);
}

SQLValue::SQLValue(const SQLValue &v)
{
    setRep(v.rep_);
}

SQLValue::SQLValue(SQLValueRep *rep)
{
    setRep(rep);
}

SQLValue::~SQLValue()
{
    clearRep();
}

void SQLValue::
setRep(SQLValueRep *rep)
{
    rep_ = rep;

    rep_->refCount_++;
}

void SQLValue::clearRep()
{
    assert(rep_->refCount_ > 0);

    if (--rep_->refCount_ == 0)
	delete rep_;
}

const SQLValue & SQLValue::operator=(const SQLValue &v)
{
    clearRep();

    setRep(v.rep_);

    return *this;
}

std::string SQLValue::asString() const
{
    std::string str;

    rep_->toString(str);

    return str;
}

bool SQLValue::asBoolean() const
{
    SQLBooleanValue bool_rep;

    if (rep_->isSameType(&bool_rep))
    {
        SQLBooleanValue *brep = (SQLBooleanValue *)rep_;

	return brep->getValue();
    }

    std::string str = asString();

    bool_rep.fromString(str);

    return bool_rep.getValue();
}

int SQLValue::asInteger() const
{
    SQLIntegerValue int_rep;

    if (rep_->isSameType(&int_rep))
    {
        SQLIntegerValue *irep = (SQLIntegerValue *)rep_;

	return irep->getValue();
    }

    std::string str = asString();

    int_rep.fromString(str);

    return int_rep.getValue();
}

double SQLValue::asReal() const
{
    SQLRealValue real_rep;

    if (rep_->isSameType(&real_rep))
    {
        SQLRealValue *rrep = (SQLRealValue *)rep_;

	return rrep->getValue();
    }

    std::string str = asString();

    real_rep.fromString(str);

    return real_rep.getValue();
}

#if SQL_DATE_SUPPORT
time_t SQLValue::asDateTime() const
{
    SQLDateTimeValue datetime_rep;

    if (rep_->isSameType(&datetime_rep))
    {
        SQLDateTimeValue *rrep = (SQLDateTimeValue *)rep_;

	return rrep->getValue();
    }

    std::string str = asString();

    datetime_rep.fromString(str);

    return datetime_rep.getValue();
}
#endif

bool SQLValue::fromString(std::string str)
{
    return rep_->fromString(str);
}

bool SQLValue::isSameType(const SQLValue &v) const
{
    return rep_->isSameType(v.rep_);
}

// Return true if the object is of void type
bool SQLValue::isVoid()
{
    if (void_rep_ == 0)
    {
	void_rep_ = new SQLVoidValue;
	// Bump the reference count so it will not be destroyed.
	void_rep_->refCount_++;
    }

    return rep_->isSameType(void_rep_);
}

// Return true if the object is of void type
bool SQLValue::isException() const
{
    SQLExceptionValue e("");

    return rep_->isSameType(&e);
}

// Change the type of this to match the type of the given value.
bool SQLValue::typeConvert(const SQLValue &v)
{
    if (isSameType(v))
        return true;

    SQLValueRep *old_rep = rep_;

    // Construct a new object of the correct type
    SQLValueRep *rep = v.rep_->clone();

    if (rep == 0)
        return false;

    // Now copy the value of the old rep into the new one.
    std::string val;

    old_rep->toString(val);

    if (!rep->fromString(val))
    {
	delete rep;
        return false;
    }

    clearRep();
    setRep(rep);

    return true;
}

int SQLValue::compare(SQLValue v)
{
    return rep_->compare(v.rep_);
}

bool SQLValue::operator==(SQLValue v)
{
    return compare(v) == 0;
}

bool SQLValue::operator!=(SQLValue v)
{
    return compare(v) != 0;
}

// Perform the given arithmetic operation and return the result
SQLValue SQLValue::binaryOperation(SQLValue v2, char op)
{
    return rep_->binaryOperation(v2.rep_, op);
}

SQLValue SQLValue::unaryOperation(char op)
{
    return rep_->unaryOperation(op);
}

// SQLValueRep definition.
SQLValueRep::SQLValueRep()
: refCount_(0)
{
}

SQLValueRep::~SQLValueRep()
{
}

bool SQLValueRep::isSameType(SQLValueRep *rep) const
{
    if (this == rep)
	return true;
    else
	return typeUniqueTag() == rep->typeUniqueTag();
}

SQLValueRep * SQLValueRep::illegalOperation(char op)
{
    std::string ops(&op, 1);

    return new SQLExceptionValue(std::string("Illegal ") + typeAsString() +
				 "operation '" + ops + "'");
}


// Derived variable classes
bool SQLVoidValue::fromString(std::string)
{
    return true;
}

void SQLVoidValue::toString(std::string &s)
{
    s = "";
}

std::string SQLVoidValue::typeAsString() const
{
    return "Void";
}

SQLValueRep * SQLVoidValue::clone() const
{
    SQLVoidValue *v = new SQLVoidValue;

    return v;
}

void * SQLVoidValue::typeUniqueTag() const
{
    // Return the functions address as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLVoidValue::compare(SQLValueRep *) const
{
    // Void values always compare as unequal
    return 1;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLVoidValue::binaryOperation(SQLValueRep *v2, char op)
{
    // Any operation applied to a void will yield a void
    return this;
}

SQLValueRep * SQLVoidValue::unaryOperation(char op)
{
    // Any operation applied to a void will yield a void
    return this;
}

// Definition of the boolean type
SQLBooleanValue::SQLBooleanValue()
: value(false)
{
}

SQLBooleanValue::SQLBooleanValue(bool value_)
: value(value_)
{
}

bool SQLBooleanValue::fromString(std::string str)
{
    char c;

    if (str.length() > 0)
	c = str[0U];
    else
	c = '\0';

    if (c == 'T' || c == 't' || c == 'y' || c == 'Y' ||	c == '1')
        value = true;
    else if (c == 'F' || c == 'f' || c == 'n' || c == 'N' || c == '0')
        value = false;
    else
        return false;

    return true;
}

void SQLBooleanValue::toString(std::string &s)
{
    s = value ? "True" : "False";
}

std::string SQLBooleanValue::typeAsString() const
{
    return "Boolean";
}

SQLValueRep * SQLBooleanValue::clone() const
{
    SQLBooleanValue *v = new SQLBooleanValue;

    v->value = value;

    return v;
}

void * SQLBooleanValue::typeUniqueTag() const
{
    // Return the address of a static variable as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLBooleanValue::compare(SQLValueRep *rep) const
{
    assert(isSameType(rep));

    SQLBooleanValue *r = (SQLBooleanValue *)rep;

    return value - r->value;
}

bool SQLBooleanValue::getValue() const
{
    return value;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLBooleanValue::binaryOperation(SQLValueRep *v2, char op)
{
    bool b;
    SQLBooleanValue *r = (SQLBooleanValue *)v2;
    switch (op)
    {
    case '+':
	b = value | r->value;
	break;
    case '*':
	b = value & r->value;
	break;
    default:
	return illegalOperation(op);
	break;
    }

    return new SQLBooleanValue(b);
}

SQLValueRep * SQLBooleanValue::unaryOperation(char op)
{
    bool b;

    switch (op)
    {
    case '-':
	b = !value;
	break;
    default:
	return illegalOperation(op);
	break;
    }

    return new SQLBooleanValue(b);
}

// Definition of the integer type
SQLIntegerValue::SQLIntegerValue()
: value(0)
{
}

SQLIntegerValue::SQLIntegerValue(int value_)
: value(value_)
{
}

bool SQLIntegerValue::fromString(std::string s)
{
    std::stringstream ss(s);

    long v;

    if (!(ss >> v))
    {
	// Try to convert using floating point
	double d;

	if (!(ss >> d))
	    return false;

	v = long(d + 0.5);
    }

    value = v;

    return true;
}

void SQLIntegerValue::toString(std::string &s)
{
    std::stringstream ss;
    ss << value;

    s = ss.str();
}

std::string SQLIntegerValue::typeAsString() const
{
    return "Integer";
}

SQLValueRep * SQLIntegerValue::clone() const
{
    SQLIntegerValue *v = new SQLIntegerValue;

    v->value = value;

    return v;
}

void * SQLIntegerValue::typeUniqueTag() const
{
    // Return the address of a static variable as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLIntegerValue::compare(SQLValueRep *rep) const
{
    assert(isSameType(rep));

    SQLIntegerValue *r = (SQLIntegerValue *)rep;

    return value - r->value;
}

int SQLIntegerValue::getValue() const
{
    return value;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLIntegerValue::binaryOperation(SQLValueRep *v2, char op)
{
    int val;
    SQLIntegerValue *r = (SQLIntegerValue *)v2;
    switch (op)
    {
    case '+':
	val = value + r->value;
	break;
    case '-':
	val = value - r->value;
	break;
    case '*':
	val = value * r->value;
	break;
    case '/':
	val = value / r->value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLIntegerValue(val);
}

SQLValueRep * SQLIntegerValue::unaryOperation(char op)
{
    int val;

    switch (op)
    {
    case '-':
	val = -value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLIntegerValue(val);
}

// Definition of the Real type
SQLRealValue::SQLRealValue()
: value(0.0)
{
}

SQLRealValue::SQLRealValue(double value)
: value(value)
{
}

bool SQLRealValue::fromString(std::string s)
{
    double v;
    std::stringstream ss(s);

    if (!(ss >> v))
	return false;

    value = v;

    return true;
}

void SQLRealValue::toString(std::string &s)
{
    std::stringstream ss;
    ss << value;

    s = ss.str();
}

std::string SQLRealValue::typeAsString() const
{
    return "Real";
}

SQLValueRep * SQLRealValue::clone() const
{
    SQLRealValue *v = new SQLRealValue;

    v->value = value;

    return v;
}

void * SQLRealValue::typeUniqueTag() const
{
    // Return the address of a static variable as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLRealValue::compare(SQLValueRep *rep) const
{
    assert(isSameType(rep));

    SQLRealValue *r = (SQLRealValue *)rep;

    double d = value - r->value;
    if (d > 0)
	return 1;
    else if (d < 0)
	return -1;
    else
	return 0;
}

double SQLRealValue::getValue() const
{
    return value;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLRealValue::binaryOperation(SQLValueRep *v2, char op)
{
    double val;
    SQLRealValue *r = (SQLRealValue *)v2;
    switch (op)
    {
    case '+':
	val = value + r->value;
	break;
    case '-':
	val = value - r->value;
	break;
    case '*':
	val = value * r->value;
	break;
    case '/':
	val = value / r->value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLRealValue(val);
}

SQLValueRep * SQLRealValue::unaryOperation(char op)
{
    double val;
    switch (op)
    {
    case '-':
	val = -value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLRealValue(val);
}

// Definition of the string type
SQLStringValue::SQLStringValue()
{
}

SQLStringValue::SQLStringValue(std::string value_)
: value(value_)
{
}

// Static function to turn on/off case sensitive string compares. Default is on
bool SQLStringValue::caseInsensitive = false;

void SQLStringValue::setCaseInsensitive(bool b)
{
    caseInsensitive = b;
}

bool SQLStringValue::fromString(std::string s)
{
    value = s;

    return true;
}

void SQLStringValue::toString(std::string &s)
{
    s = value;
}

std::string SQLStringValue::typeAsString() const
{
    return "String";
}

SQLValueRep * SQLStringValue::clone() const
{
    SQLStringValue *v = new SQLStringValue;

    v->value = value;

    return v;
}

void * SQLStringValue::typeUniqueTag() const
{
    // Return the address of a static variable as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLStringValue::compare(SQLValueRep *rep) const
{
    assert(isSameType(rep));

    SQLStringValue *r = (SQLStringValue *)rep;

    if (caseInsensitive)
    {
#ifdef __WIN32__
	return stricmp(value.c_str(), r->value.c_str());
#else
	return strcasecmp(value.c_str(), r->value.c_str());
#endif
    }
    else
	return strcmp(value.c_str(), r->value.c_str());
}

std::string SQLStringValue::getValue() const
{
    return value;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLStringValue::binaryOperation(SQLValueRep *v2, char op)
{
    std::string s;
    SQLStringValue *r = (SQLStringValue *)v2;
    switch (op)
    {
    case '+':
	s = value + r->value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLStringValue(s);
}

SQLValueRep * SQLStringValue::unaryOperation(char op)
{
    return illegalOperation(op);
}

#if SQL_DATE_SUPPORT
std::string SQLDateTimeValue::format = "%Y-%m-%d %H:%M:%S";

SQLDateTimeValue::SQLDateTimeValue()
{
}

SQLDateTimeValue::SQLDateTimeValue(time_t value_)
: value(value_)
{
}

bool SQLDateTimeValue::fromString(std::string s)
{
    struct tm tmbuf;
    memset(&tmbuf, 0, sizeof(tmbuf));
    char *p = strptime(s.c_str(), format.c_str(), &tmbuf);
    if (p == 0)
    {
#if DEBUG_TIME
        printf("strptime failed for '%s' with format '%s'\n",
	       s.c_str(), format.c_str());
#endif
        return false;
    }

    value = mktime(&tmbuf);

#if DEBUG_TIME
    printf("SQLDateTimeValue %s is %ld\n", s.c_str(), value);
#endif

    return (value != -1);
}

void SQLDateTimeValue::toString(std::string &s)
{
    struct tm tmbuf;
    if (localtime_r(&value, &tmbuf) == 0)
        s = "";

    char buf[2048];
    strftime(buf, sizeof(buf), format.c_str(), &tmbuf);

    s = buf;
}

std::string SQLDateTimeValue::typeAsString() const
{
    return "DateTime";
}

SQLValueRep * SQLDateTimeValue::clone() const
{
    SQLDateTimeValue *v = new SQLDateTimeValue;

    v->value = value;

    return v;
}

void * SQLDateTimeValue::typeUniqueTag() const
{
    // Return the address of a static variable as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLDateTimeValue::compare(SQLValueRep *rep) const
{
    assert(isSameType(rep));

    SQLDateTimeValue *r = (SQLDateTimeValue *)rep;

    return (long)value - (long)r->value;
}

time_t SQLDateTimeValue::getValue() const
{
    return value;
}

void SQLDateTimeValue::setFormat(const std::string &format_)
{
    format = format_;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLDateTimeValue::binaryOperation(SQLValueRep *v2, char op)
{
    unsigned long seconds;

    SQLDateTimeValue *r = (SQLDateTimeValue *)v2;
    switch (op)
    {
    case '+':
	seconds = value + r->value;
	break;
    case '-':
	seconds = value - r->value;
	break;
    case '*':
	seconds = value * r->value;
	break;
    case '/':
	seconds = value / r->value;
	break;
    default:
	return illegalOperation(op);
    }

    return new SQLDateTimeValue(seconds);
}

SQLValueRep * SQLDateTimeValue::unaryOperation(char op)
{
    return illegalOperation(op);
}
#endif

// Exception Value class
SQLExceptionValue::SQLExceptionValue(std::string exception)
: exception_(exception)
{
}

bool SQLExceptionValue::fromString(std::string)
{
    return false;
}

void SQLExceptionValue::toString(std::string &s)
{
    s = exception_;
}

std::string SQLExceptionValue::typeAsString() const
{
    return "Exception";
}

SQLValueRep * SQLExceptionValue::clone() const
{
    SQLExceptionValue *e = new SQLExceptionValue(exception_);

    return e;
}

void * SQLExceptionValue::typeUniqueTag() const
{
    // Return the functions address as the type unique tag.
    static void *tag;
    return (void *)&tag;
}

int SQLExceptionValue::compare(SQLValueRep *v2) const
{
    // Exceptions always compare as unequal
    return 1;
}

// Perform the given arithmetic operation and return the result
SQLValueRep * SQLExceptionValue::binaryOperation(SQLValueRep *v2, char op)
{
    // Any operation applied to an exception will yield an exception
    return this;
}

SQLValueRep * SQLExceptionValue::unaryOperation(char op)
{
    // Any operation applied to an exception will yield an exception
    return this;
}
