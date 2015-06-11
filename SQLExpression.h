/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLExpression.h
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Expression Tree
 */
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "SQLValue.h"
#include <regex.h>

class SQLContext;

/**
 * SQLExpression evaluation classes.
 */
class SQLExpression
{
public:
    SQLExpression();

    virtual SQLValue evaluate(SQLContext &context) = 0;

    /** Show the parse tree as a string. This is useful for debugging */
    virtual std::string asString() const = 0;
    virtual const char *shortName() const = 0;

    static SQLValue SQLTrueValue;
    static SQLValue SQLFalseValue;

    void getRef() { refCount++; }

    void releaseRef() 
    {
        refCount--;

        if (refCount <= 0)
            delete this;
    }

protected:
    virtual ~SQLExpression();
    int refCount;
};


/**
 * List of SQLExpression objects.
 */
class SQLExpressionList
{
public:
    SQLExpressionList();
    virtual ~SQLExpressionList();

    void addExpression(SQLExpression *e);

    int numExpressions();
    SQLExpression *expressionNumber(int i);

    std::string asString() const;

protected:
    int numExpr;
    SQLExpression **expressions;
};


/**
 * Unary SQLExpression.
 */
class SQLUnaryExpression
: public SQLExpression
{
public:
    SQLUnaryExpression(SQLExpression *expr);
    virtual std::string asString() const;
    virtual const char *shortName() const;

protected:
    virtual ~SQLUnaryExpression();
    SQLExpression *expr;
};

/**
 * Binary SQLExpression.
 */
class SQLBinaryExpression
: public SQLExpression
{
public:
    SQLBinaryExpression(SQLExpression *expr1, SQLExpression *expr2);
    virtual std::string asString() const;
    virtual const char *shortName() const;

protected:
    virtual ~SQLBinaryExpression();
    SQLExpression *expr1;
    SQLExpression *expr2;

    bool evaluateSiblings(SQLContext &context,
			  SQLValue &v1, SQLValue &v2);
};

/**
 * Terminal SQLExpression.
 */
class SQLTerminalExpression
: public SQLExpression
{
    virtual std::string asString() const;
    virtual const char *shortName() const;
};

/**
 * Equals SQLExpression.
 */
class SQLEqualsExpression
: public SQLBinaryExpression
{
public:
    SQLEqualsExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Not-Equals SQLExpression.
 */
class SQLNotEqualsExpression
: public SQLBinaryExpression
{
public:
    SQLNotEqualsExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Less-Than SQLExpression.
 */
class SQLLessThanExpression
: public SQLBinaryExpression
{
public:
    SQLLessThanExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Greater-Than SQLExpression.
 */
class SQLGreaterThanExpression
: public SQLBinaryExpression
{
public:
    SQLGreaterThanExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Less-Than or Equals SQLExpression.
 */
class SQLLessEqualsExpression
: public SQLBinaryExpression
{
public:
    SQLLessEqualsExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Greater-Than or Equals SQLExpression.
 */
class SQLGreaterEqualsExpression
: public SQLBinaryExpression
{
public:
    SQLGreaterEqualsExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * 'AND' SQLExpression.
 */
class SQLAndExpression
: public SQLBinaryExpression
{
public:
    SQLAndExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * 'OR' SQLExpression.
 */
class SQLOrExpression
: public SQLBinaryExpression
{
public:
    SQLOrExpression(SQLExpression *expr1, SQLExpression *expr2)
        : SQLBinaryExpression(expr1, expr2) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Operation SQLExpression.
 */
class SQLOperationExpression
: public SQLBinaryExpression
{
public:
    SQLOperationExpression(SQLExpression *expr1, SQLExpression *expr2,
			   char op_)
        : SQLBinaryExpression(expr1, expr2), op(op_) { ; }
    virtual std::string asString() const;
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
protected:
    char op;
};

/**
 * 'NOT' SQLExpression.
 */
class SQLNotExpression
: public SQLUnaryExpression
{
public:
    SQLNotExpression(SQLExpression *expr)
        : SQLUnaryExpression(expr) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Negation SQLExpression.
 */
class SQLNegateExpression
: public SQLUnaryExpression
{
public:
    SQLNegateExpression(SQLExpression *expr)
        : SQLUnaryExpression(expr) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * 'IN' SQLExpression.
 */
class SQLInExpression
: public SQLExpression
{
public:
    SQLInExpression(SQLExpression *expr, SQLExpressionList *list);

    virtual SQLValue evaluate(SQLContext &context);

    /** Show the parse tree as a string. This is useful for debugging */
    virtual std::string asString() const;
    virtual const char *shortName() const;

protected:
    ~SQLInExpression();
    SQLExpression *expr;
    SQLExpressionList *list;
};

/**
 * 'Like' SQLExpression.
 */
class SQLLikeExpression
: public SQLUnaryExpression
{
public:
    SQLLikeExpression(SQLExpression *expr,
                      const std::string &pattern,
		      const std::string &escape);
    const char *shortName() const;
    std::string asString() const;

    SQLValue evaluate(SQLContext &context);
protected:
    ~SQLLikeExpression();
    regex_t regex;
};

/**
 * Function SQLExpression.
 */
class SQLFunctionExpression
: public SQLExpression
{
public:
    SQLFunctionExpression(const std::string &class_name,
                          const std::string &member_name,
			  SQLExpressionList *list);

    virtual SQLValue evaluate(SQLContext &context);

    /** Show the parse tree as a string. This is useful for debugging */
    virtual std::string asString() const;
    virtual const char *shortName() const;

protected:
    ~SQLFunctionExpression();
    std::string className;
    std::string memberName;
    SQLExpressionList *list;
};

/**
 * Null SQLExpression.
 */
class SQLNullExpression
: public SQLUnaryExpression
{
public:
    SQLNullExpression(SQLExpression *expr)
        : SQLUnaryExpression(expr) { ; }
    virtual const char *shortName() const;

    SQLValue evaluate(SQLContext &context);
};

/**
 * Variable SQLExpression.
 */
class SQLVariableExpression
: public SQLTerminalExpression
{
public:
    SQLVariableExpression(const std::string &class_name,
                          const std::string &member_name)
        : className(class_name), memberName(member_name) { ; }
    virtual const char *shortName() const;
    virtual std::string asString() const;

    SQLValue evaluate(SQLContext &context);
protected:
    std::string className;
    std::string memberName;
};

/**
 * Value SQLExpression.
 */
class SQLValueExpression
: public SQLTerminalExpression
{
public:
    SQLValueExpression(SQLValue value);
    virtual const char *shortName() const;
    virtual std::string asString() const;

    SQLValue evaluate(SQLContext &context);

    // Extension to allow caching the type conversions
    SQLValue evaluateAsType(const SQLValue &v2);

protected:
    SQLValue value;
    SQLValue typedValue;
};

#endif
