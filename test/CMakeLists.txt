add_executable(SQLValueTest SQLValueTest.cpp)
target_link_libraries(SQLValueTest
    SimpleSQL
)
add_test(SQLValueTest SQLValueTest)

add_executable(ExpressionTest ExpressionTest.cpp)
target_link_libraries(ExpressionTest
    SimpleSQL
)
add_test(ExpressionTest ExpressionTest)

add_executable(SQLContextTest SQLContextTest.cpp)
target_link_libraries(SQLContextTest
    SimpleSQL
)
add_test(SQLContextTest SQLContextTest)

add_executable(SQLParseTest SQLParseTest.cpp)
target_link_libraries(SQLParseTest
    SimpleSQL
)
add_test(SQLParseTest SQLParseTest)

add_executable(function_test function_test.cpp)
target_link_libraries(function_test
    SimpleSQL
)
add_test(function_test function_test)

add_executable(operation_test operation_test.cpp)
target_link_libraries(operation_test
    SimpleSQL
)
add_test(operation_test operation_test)

# These need SQL_DATE_SUPPORT

add_executable(query_test query_test.cpp)
target_link_libraries(query_test
    SimpleSQL
)
add_test(query_test query_test)

add_executable(performance_test performance_test.cpp)
target_link_libraries(performance_test
    SimpleSQL
)
add_test(performance_test performance_test)
