cmake_minimum_required(VERSION 2.8)
project(NUT)

include_directories(
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/catch/include
)

add_executable(Tests
    Tests/test_functions.cpp
)

enable_testing()

add_test(NAME Tests COMMAND Tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
