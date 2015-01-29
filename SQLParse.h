/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLParse.h
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Parse SQL expressions into a expression tree
 */
#ifndef SQLPARSE_H
#define SQLPARSE_H

#include <sys/types.h>
#include <string>

class SQLExpression;

/**
 * Represent an error during parsing SQL.
 */
class SQLParseError
{
public:
    SQLParseError();

    /** Return the offset in the parse string of the error */
    int index() const;
    void index(int i);

    /** Error code string */
    std::string code() const;
    void code(std::string c);
private:
    int index_;
    std::string code_;
};


/**
 * Parse the given string of file and return an expression
 */
class SQLParse
{
public:
    SQLParse();
    ~SQLParse();

    bool parse(std::string str);

    SQLExpression *expression() const;
    void clearExpression();

    int numErrors() const;
    SQLParseError *errorNumber(int i);

    /** Return all of the error codes as a string */
    std::string errorString();

private:
    std::string parse_string_;
    size_t index_;

    SQLExpression *expression_;


    /** Support routines for yacc */
    void addError(std::string err);
    static SQLParse *current_parser_;
    void setExpression(SQLExpression *e);

    /** Support for error handling */
    int num_errors_;
    enum { MAX_ERRORS = 20 };
    SQLParseError errors_[MAX_ERRORS];
    SQLParseError *newError();
    SQLParseError *currentError();
    void clearErrors();

friend void SimpleSQL_error(const char *err);
friend int SimpleSQL_parse();
};

#endif
