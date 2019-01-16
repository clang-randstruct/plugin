#define CATCH_CONFIG_MAIN
#include "catch.hpp"

int add(int a, int b) { return a + b; }

TEST_CASE("Chaining math functions", "[functions]") { REQUIRE(add(1, 2) == 3); }
