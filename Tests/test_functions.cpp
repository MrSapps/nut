#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <nut/nut.hpp>

bool function_with_no_arguments_and_no_return_value_called = false;
void function_with_no_arguments_and_no_return_value()
{
    function_with_no_arguments_and_no_return_value_called = true;
}


TEST_CASE("stack", "push_get")
{
    nut::vm vm;

    nut::stack s(vm.mVmHandle);
    s.push("blah", 4);

    REQUIRE(strcmp("blah", s.get_string(s.top())) == 0);

}

TEST_CASE("functions", "check calling function with no arguments and no return value") 
{
    nut::vm vm;

    vm.script("print(\"test\");");

    vm.set_function("function_with_no_arguments_and_no_return_value", function_with_no_arguments_and_no_return_value);

    size_t count = nut::getArgumentCount(function_with_no_arguments_and_no_return_value).value;

    REQUIRE(function_with_no_arguments_and_no_return_value_called == false);
    vm.script("function_with_no_arguments_and_no_return_value();");
    REQUIRE(function_with_no_arguments_and_no_return_value_called == true);
}
