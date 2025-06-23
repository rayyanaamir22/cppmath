#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/numerics/root.hpp"
#include <cmath>

TEST_CASE("Newton's sqrt method") {
    // Test basic cases
    CHECK(std::abs(cppmath::numerics::newton_sqrt(4.0) - 2.0) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_sqrt(9.0) - 3.0) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_sqrt(16.0) - 4.0) < 1e-10);
    
    // Test edge cases
    CHECK(cppmath::numerics::newton_sqrt(0.0) == 0.0);
    CHECK(std::abs(cppmath::numerics::newton_sqrt(1.0) - 1.0) < 1e-10);
    
    // Test irrational numbers
    CHECK(std::abs(cppmath::numerics::newton_sqrt(2.0) - std::sqrt(2.0)) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_sqrt(3.0) - std::sqrt(3.0)) < 1e-10);
    
    // Test negative number (should throw)
    CHECK_THROWS_AS(cppmath::numerics::newton_sqrt(-1.0), std::invalid_argument);
}

TEST_CASE("Newton's cbrt method") {
    // Test basic cases
    CHECK(std::abs(cppmath::numerics::newton_cbrt(8.0) - 2.0) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_cbrt(27.0) - 3.0) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_cbrt(64.0) - 4.0) < 1e-10);
    
    // Test edge cases
    CHECK(cppmath::numerics::newton_cbrt(0.0) == 0.0);
    CHECK(std::abs(cppmath::numerics::newton_cbrt(1.0) - 1.0) < 1e-10);
    
    // Test irrational numbers
    CHECK(std::abs(cppmath::numerics::newton_cbrt(2.0) - std::cbrt(2.0)) < 1e-10);
    CHECK(std::abs(cppmath::numerics::newton_cbrt(3.0) - std::cbrt(3.0)) < 1e-10);
    
    // Test negative numbers (should work for cube root)
    CHECK(std::abs(cppmath::numerics::newton_cbrt(-8.0) - (-2.0)) < 1e-10);
} 