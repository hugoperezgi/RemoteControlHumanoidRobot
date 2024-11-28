cmake_minimum_required(VERSION 3.10)

add_executable(QueryTest test_queryGenerator.cpp)
target_link_libraries(QueryTest ControllersLib)
add_test(NAME testAllQueries COMMAND QueryTest)
