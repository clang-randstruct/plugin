#include "catch.hpp"

int mult(int a, int b) {
	return a*b;
}

int add(int a, int b) {
	return a+b;
}

TEST_CASE( "Chaining math functions", "[functions]") {
	REQUIRE( mult(2, 4) == 9 );
	REQUIRE( add(1, 2) == 3 );
}
