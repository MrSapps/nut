#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <nut/nut.hpp>

bool function_with_no_arguments_and_no_return_value_called = false;
void function_with_no_arguments_and_no_return_value()
{
    function_with_no_arguments_and_no_return_value_called = true;
}

TEST_CASE("functions", "check calling function with no arguments and no return value") 
{
    nut::vm vm;

    vm.set_function("function_with_no_arguments_and_no_return_value", function_with_no_arguments_and_no_return_value);

    REQUIRE(function_with_no_arguments_and_no_return_value_called == false);
    vm.script("function_with_no_arguments_and_no_return_value()");
    REQUIRE(function_with_no_arguments_and_no_return_value_called == true);
}
