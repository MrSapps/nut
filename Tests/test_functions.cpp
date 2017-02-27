#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <nut/nut.hpp>

bool function_with_no_arguments_and_no_return_value_called = false;
void function_with_no_arguments_and_no_return_value()
{
    function_with_no_arguments_and_no_return_value_called = true;
}


TEST_CASE("stack", "push_read")
{
    nut::vm vm;

    nut::stack s(vm.mVmHandle);

    // String
    s.push("blah");
    REQUIRE(strcmp("blah", s.Read<const SQChar*>(s.top())) == 0);

    // Float
    s.push(1998.1f);
    REQUIRE(1998.1f == s.Read<float>(s.top()));

    // Integer

    // Pointer

    // bool

    // null

    // Multi push
    s.push(1.0f, "hello", 2.0f, 3.0f);

    // Multi read
    float v1;
    const SQChar* v2;
    float v3;
    float v4;
    std::tie(v1, v2, v3, v4) = s.pop<float, const SQChar*, float, float>();
}

TEST_CASE("functions", "check calling function with no arguments and no return value") 
{
    nut::vm vm;

    vm.script("print(\"test\");");

    vm.set_function("function_with_no_arguments_and_no_return_value", function_with_no_arguments_and_no_return_value);

    size_t count = nut::getArgumentCount(function_with_no_arguments_and_no_return_value).value;

    REQUIRE(function_with_no_arguments_and_no_return_value_called == false);
    vm.script("function_with_no_arguments_and_no_return_value();");
    //REQUIRE(function_with_no_arguments_and_no_return_value_called == true);
}
