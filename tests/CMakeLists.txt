cmake_minimum_required(VERSION 3.0.0)

project(tests)

enable_testing()

#set(test1_source test.cpp)
#add_executable(test1 ${test1_source})

add_executable(test1 test.cpp)

target_link_libraries(test1 oscillation)

#add_test(NAME Test1 COMMAND test1)

add_test(NAME Test1 COMMAND test1)