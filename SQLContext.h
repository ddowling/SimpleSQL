/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLContext.h
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Context pattern to map variables and functions to values
 */
#ifndef SQLCONTEXT_H
#define SQLCONTEXT_H

#include "SQLValue.h"

class SQLContext
{
public:
    /** Create a context */
    SQLContext();
    virtual ~SQLContext();

    /**
     * chain another context to be evaluated after this context
     * is evaluated
     */
    void chain(SQLContext *c);
    SQLContext *getNextInChain() const;

    /**
     * Lookup the a variable optionally in a class and return its
     * value.
     */
    virtual SQLValue variableLookup(const std::string &class_name,
				    const std::string &member_name) const;

    /** Evaluate a function and return its value. */
    virtual SQLValue functionLookup(const std::string &class_name,
				    const std::string &member_name,
				    int num_args, SQLValue *arguments);

protected:
    /** Evaluate some default SQL functions. */
    SQLValue defaultFunctionLookup(const std::string &class_name,
				   const std::string &member_name,
				   int num_args, SQLValue *arguments);

    SQLContext *nextInChain;
};

#endif
