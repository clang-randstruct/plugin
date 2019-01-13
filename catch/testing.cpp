#define CATCH_CONFIG_MAIN //Used so Catch will generate "main()"
                          //must be included in strictly one '.cpp' file for Catch to find
#include "catch.hpp"


int mult(int a, int b){
  return a*b;
}

int add(int a, int b){
  return a+b;
}

TEST_CASE( "Chaining Math Functions", "[functions]" ) {
  REQUIRE( mult(2, 4) == 8 );
  REQUIRE( add(1, 2) == 3 );
}

