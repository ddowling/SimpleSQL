/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLValue.h
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Variant value class for SQL values
 */
#ifndef SQLVALUE_H
#define SQLVALUE_H

#include <string>
#include <assert.h>

#define SQL_DATE_SUPPORT 1

#if SQL_DATE_SUPPORT
#include <time.h>
#endif

class SQLValueRep;
class SQLVoidValue;

/**
 * Implementation of the SQL variable types
 */
class SQLValueRep
{
friend class SQLValue;
public:
    SQLValueRep();
    virtual ~SQLValueRep();

    /** Sets the variable from the given string. */
    virtual bool fromString(std::string s) = 0;

    /** Extracts the variable into a string. */
    virtual void toString(std::string &s) = 0;

    /** Return the type as a printable string. */
    virtual std::string typeAsString() const = 0;

    /** clone the current object and return a pointer to the new instance */
    virtual SQLValueRep *clone() const = 0;

    /** Return TRUE if the objects are the same type. */
    bool isSameType(SQLValueRep *rep) const;

    /**
     * Compare this with argument and return
     *      -1 if this < v
     *      0 if this == v
     *      1 if this > v
     */
    virtual int compare(SQLValueRep *rep) const = 0;

    /** Perform the given arithmetic operation and return the result */
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op) = 0;
    virtual SQLValueRep *unaryOperation(char op) = 0;

protected:
    SQLValueRep *illegalOperation(char op);

private:
    /**
     * Return a unique number for the object type. This allows
     * quick type comparison.
     */
    virtual void *typeUniqueTag() const = 0;

    int refCount_;
};

/**
 * Represent the different SQL value types.
 *
 */
class SQLValue
{
public:
    SQLValue();
    SQLValue(const SQLValue &v);
    SQLValue(SQLValueRep *rep);
    ~SQLValue();

    const SQLValue &operator=(const SQLValue &v);

    /** Return the value as some common types */
    std::string asString() const;
    bool asBoolean() const;
    int asInteger() const;
    double asReal() const;

#if SQL_DATE_SUPPORT
    time_t asDateTime() const;
#endif

    bool fromString(std::string str);

    /** Return true if the objects are of the same type */
    bool isSameType(const SQLValue &v) const;

    /** Return true if the object is void
     * Needs to increase void_rep reference count so cannot be const
     */
    bool isVoid();

    /** Return true if the object is an exception */
    bool isException() const;

    /**
     * Compare this with argument and return
     *      -1 if this < v
     *      0 if this == v
     *      1 if this > v
     */
    int compare(SQLValue v);

    bool operator==(SQLValue v);
    bool operator!=(SQLValue v);

    /** Change the type of this to match the type of the given value. */
    bool typeConvert(const SQLValue &v);

    /** Perform the given arithmetic operation and return the result */
    SQLValue binaryOperation(SQLValue v2, char op);
    SQLValue unaryOperation(char op);

private:
    SQLValueRep *rep_;
    static SQLVoidValue *void_rep_;

    void setRep(SQLValueRep *rep)
    {
        rep_ = rep;

        rep_->refCount_++;
    }

    void clearRep()
    {
        assert(rep_->refCount_ > 0);

        if (--rep_->refCount_ == 0)
            delete rep_;
    }
};

/**
 * Represent the SQL void value.
 */
class SQLVoidValue
: public SQLValueRep
{
public:
    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

private:
    virtual void *typeUniqueTag() const;
};

/**
 * Represent the SQL boolean value.
 */
class SQLBooleanValue
: public SQLValueRep
{
public:
    SQLBooleanValue();
    SQLBooleanValue(bool value);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

    bool getValue() const;
private:
    bool value;
    virtual void *typeUniqueTag() const;
};

/**
 * Represent the SQL integer value.
 */
class SQLIntegerValue
: public SQLValueRep
{
public:
    SQLIntegerValue();
    SQLIntegerValue(int value);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

    int getValue() const;

private:
    int value;
    virtual void *typeUniqueTag() const;
};

/**
 * Represent the SQL real value.
 */
class SQLRealValue
: public SQLValueRep
{
public:
    SQLRealValue();
    SQLRealValue(double value);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

    double getValue() const;
private:
    double value;
    virtual void *typeUniqueTag() const;
};

/**
 * Represent the SQL string value.
 */
class SQLStringValue
: public SQLValueRep
{
public:
    SQLStringValue();
    SQLStringValue(std::string value);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

    std::string getValue() const;

    static void setCaseInsensitive(bool s);
private:
    std::string value;
    virtual void *typeUniqueTag() const;

    static bool caseInsensitive;
};

#if SQL_DATE_SUPPORT
/**
 * Represent the SQL datetime value.
 */
class SQLDateTimeValue
: public SQLValueRep
{
public:
    SQLDateTimeValue();
    SQLDateTimeValue(time_t value);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

    time_t getValue() const;

    static void setFormat(const std::string &fmt);
private:
    static std::string format;

    time_t value;
    virtual void *typeUniqueTag() const;
};
#endif

/**
 * This class is used to pass exception information up the evaluation
 * tree to the user
 */
class SQLExceptionValue
: public SQLValueRep
{
public:
    SQLExceptionValue(std::string exception);

    virtual bool fromString(std::string s);
    virtual void toString(std::string &s);

    virtual std::string typeAsString() const;
    virtual SQLValueRep *clone() const;
    virtual int compare(SQLValueRep *rep) const;
    virtual SQLValueRep *binaryOperation(SQLValueRep *v2, char op);
    virtual SQLValueRep *unaryOperation(char op);

private:
    virtual void *typeUniqueTag() const;
    std::string exception_;
};

#endif
