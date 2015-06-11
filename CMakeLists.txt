cmake_minimum_required(VERSION 2.8)

add_definitions(-DSQL_DATE_SUPPORT=1)

find_package(BISON)
find_package(FLEX)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

BISON_TARGET(SQLParser
    SQLParse_yacc.ypp
    ${CMAKE_CURRENT_BINARY_DIR}/SQLParse_yacc.cpp
)

FLEX_TARGET(SQLLexer
    SQLParse_lex.lpp
    ${CMAKE_CURRENT_BINARY_DIR}/SQLParse_lex.cpp
    # Make the lexer case insensitive
    COMPILE_FLAGS -i
)

ADD_FLEX_BISON_DEPENDENCY(SQLLexer SQLParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(SimpleSQL
    SQLContext.cpp
    SQLParse.cpp
    ${BISON_SQLParser_OUTPUTS}
    ${FLEX_SQLLexer_OUTPUTS}
    SQLExpression.cpp
    SQLValue.cpp
)

enable_testing()
add_subdirectory(test)
