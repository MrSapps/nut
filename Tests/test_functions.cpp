#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <nut/nut.hpp>

TEST_CASE("functions", "check calling function with no arguments and no return") 
{
    REQUIRE(1 == 2);
}
