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

    /** Return the line and column of the error.
     * By convention lines and columns are numbered starting from 1.
     */
    int line() const;
    void line(int l);

    int column() const;
    void column(int c);

    /** Error code string */
    const std::string &code() const;
    void code(const std::string &c);
private:
    std::string code_;
    int line_;
    int column_;
};


/**
 * Parse the given string of file and return an expression
 */
class SQLParse
{
public:
    SQLParse();
    ~SQLParse();

    bool parse(const std::string &str);

    SQLExpression *expression() const;
    void clearExpression();

    int numErrors() const;
    const SQLParseError *errorNumber(int i) const;

    /** Return all of the error codes as a string */
    std::string errorString() const;

private:
    std::string parse_string_;

    SQLExpression *expression_;


    /** Support routines for yacc */
    void addError(const std::string &err, int line, int column);
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
