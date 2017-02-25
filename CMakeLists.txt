cmake_minimum_required(VERSION 2.8)
project(NUT)

if (MSVC)
    add_definitions("-D _CRT_SECURE_NO_WARNINGS")
endif()

include_directories(
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/catch/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/squirrel/include
)

add_executable(Tests
    Tests/test_functions.cpp
)
TARGET_LINK_LIBRARIES(Tests sqstdlib_static squirrel_static)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/squirrel)

enable_testing()

add_test(NAME Tests COMMAND Tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
