/*
 * Copyright   : (c) 2010 by Open Source Solutions Pty Ltd.  All Rights Reserved
 * Project     : Core Libraries
 * File        : SQLParse.cpp
 *
 * Author      : Denis Dowling
 * Created     : 1/12/2010
 *
 * Description : Parse SQL expressions into a expression tree
 */
#include "SQLParse.h"
#include "SQLExpression.h"
#include <assert.h>
#include <sstream>

// flex buffer interface
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_bytes(const char *base, int size);
extern void yy_delete_buffer(YY_BUFFER_STATE b);


extern int SimpleSQL_parse();

// Error Handling class
SQLParseError::SQLParseError()
 : line_(0), column_(0)
{
}

int SQLParseError::line() const
{
    return line_;
}

void SQLParseError::line(int l)
{
    line_ = l;
}

int SQLParseError::column() const
{
    return column_;
}

void SQLParseError::column(int c)
{
    column_ = c;
}

// Error code string
const std::string & SQLParseError::code() const
{
    return code_;
}

void SQLParseError::code(const std::string &c)
{
    code_ = c;
}

SQLParse::SQLParse()
: expression_(0)
{
}

SQLParse::~SQLParse()
{
    setExpression(0);
}

bool SQLParse::parse(const std::string &str)
{
    parse_string_ = str;

    setExpression(0);

    current_parser_ = this;
    clearErrors();

    YY_BUFFER_STATE buffer = yy_scan_bytes((char *)parse_string_.c_str(),
					   parse_string_.size());
    if (SimpleSQL_parse() != 0)
	return false;

    yy_delete_buffer(buffer);

    return numErrors() == 0;
}

void SQLParse::clearExpression()
{
    setExpression(0);
}

SQLParse * SQLParse::current_parser_ = 0;

void SQLParse::addError(const std::string &err, int line, int column)
{
    SQLParseError *e = currentError();

    // Yacc seems to generate \'syntax error\' even when we trap normal errors
    if (e != 0 && e->code() == "syntax error")
    {
	e->code(err);
	e->line(line);
	e->column(column);
    }
    else
    {
	e = newError();

	// If cannot register another error then just return
	if (e == 0)
	    return;

	e->code(err);
	e->line(line);
	e->column(column);
    }
}

SQLParseError * SQLParse::currentError()
{
    if (num_errors_ == 0)
	return 0;

    return &(errors_[num_errors_ - 1]);
}

SQLParseError * SQLParse::newError()
{
    if (num_errors_ >= MAX_ERRORS - 1)
	return 0;

    return &(errors_[num_errors_++]);
}

const SQLParseError * SQLParse::errorNumber(int i) const
{
    if (i >= num_errors_ || i < 0)
	return 0;

    return &(errors_[i]);
}

void SQLParse::clearErrors()
{
    num_errors_ = 0;
}

int SQLParse::numErrors() const
{
    return num_errors_;
}

// Return all of the error codes as a string
std::string SQLParse::errorString() const
{
    std::stringstream s;

    for(int i = 0; i < num_errors_; i++)
    {
	const SQLParseError *err = errorNumber(i);

	s << err->code();
        if (err->line() > 0)
            s << " at " << err->line() << ":" << err->column();
        s << "\n";
    }

    return s.str();
}

void SQLParse::setExpression(SQLExpression *e)
{
    if (expression_ != 0)
	expression_->releaseRef();

    expression_ = e;

    if (expression_ != 0)
	expression_->getRef();
}

SQLExpression * SQLParse::expression() const
{
    return expression_;
}
