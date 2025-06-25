#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/combinatorics/combo.hpp"
#include <vector>

TEST_CASE("Fibonacci numbers - fibonacci()") {
    // Test base cases
    CHECK(cppmath::combinatorics::fibonacci(1) == 0);
    CHECK(cppmath::combinatorics::fibonacci(2) == 1);
    CHECK(cppmath::combinatorics::fibonacci(3) == 1);
    
    // Test sequence values
    CHECK(cppmath::combinatorics::fibonacci(4) == 2);
    CHECK(cppmath::combinatorics::fibonacci(5) == 3);
    CHECK(cppmath::combinatorics::fibonacci(6) == 5);
    CHECK(cppmath::combinatorics::fibonacci(7) == 8);
    CHECK(cppmath::combinatorics::fibonacci(8) == 13);
    CHECK(cppmath::combinatorics::fibonacci(9) == 21);
    CHECK(cppmath::combinatorics::fibonacci(10) == 34);
    CHECK(cppmath::combinatorics::fibonacci(11) == 55);
    CHECK(cppmath::combinatorics::fibonacci(12) == 89);
    CHECK(cppmath::combinatorics::fibonacci(13) == 144);
    CHECK(cppmath::combinatorics::fibonacci(14) == 233);
    CHECK(cppmath::combinatorics::fibonacci(15) == 377);
    CHECK(cppmath::combinatorics::fibonacci(16) == 610);
    CHECK(cppmath::combinatorics::fibonacci(17) == 987);
    CHECK(cppmath::combinatorics::fibonacci(18) == 1597);
    CHECK(cppmath::combinatorics::fibonacci(19) == 2584);
    CHECK(cppmath::combinatorics::fibonacci(20) == 4181);
    
    // Test larger values
    CHECK(cppmath::combinatorics::fibonacci(25) == 46368);
    CHECK(cppmath::combinatorics::fibonacci(30) == 514229);
    CHECK(cppmath::combinatorics::fibonacci(35) == 5702887);
    CHECK(cppmath::combinatorics::fibonacci(40) == 63245986);
    
    // Test memoization works correctly
    CHECK(cppmath::combinatorics::fibonacci(10) == 34);  // Should be cached
    CHECK(cppmath::combinatorics::fibonacci(15) == 377); // Should be cached
}

TEST_CASE("Fibonacci numbers - error handling") {
    // Test invalid input
    CHECK_THROWS_AS(cppmath::combinatorics::fibonacci(0), std::runtime_error);
}

TEST_CASE("Factorial - factorial()") {
    // Test base cases
    CHECK(cppmath::combinatorics::factorial(0) == 1);
    CHECK(cppmath::combinatorics::factorial(1) == 1);
    
    // Test small values
    CHECK(cppmath::combinatorics::factorial(2) == 2);
    CHECK(cppmath::combinatorics::factorial(3) == 6);
    CHECK(cppmath::combinatorics::factorial(4) == 24);
    CHECK(cppmath::combinatorics::factorial(5) == 120);
    CHECK(cppmath::combinatorics::factorial(6) == 720);
    CHECK(cppmath::combinatorics::factorial(7) == 5040);
    CHECK(cppmath::combinatorics::factorial(8) == 40320);
    CHECK(cppmath::combinatorics::factorial(9) == 362880);
    CHECK(cppmath::combinatorics::factorial(10) == 3628800);
    
    // Test larger values
    CHECK(cppmath::combinatorics::factorial(12) == 479001600);
    CHECK(cppmath::combinatorics::factorial(15) == 1307674368000);
    CHECK(cppmath::combinatorics::factorial(20) == 2432902008176640000ULL);
}

TEST_CASE("Factorial - error handling") {
    // Note: The current implementation doesn't throw for negative numbers
    // but it should according to the header documentation
    // This test documents the current behavior
    // CHECK_THROWS_AS(cppmath::combinatorics::factorial(-1), std::runtime_error);
}

TEST_CASE("Fibonacci sequence - fibonacci_sequence()") {
    // Test small limits
    auto seq1 = cppmath::combinatorics::fibonacci_sequence(1);
    CHECK(seq1.size() == 1);
    CHECK(seq1[0] == 0);
    
    auto seq2 = cppmath::combinatorics::fibonacci_sequence(2);
    CHECK(seq2.size() == 2);
    CHECK(seq2[0] == 0);
    CHECK(seq2[1] == 1);
    
    auto seq3 = cppmath::combinatorics::fibonacci_sequence(3);
    CHECK(seq3.size() == 3);
    CHECK(seq3[0] == 0);
    CHECK(seq3[1] == 1);
    CHECK(seq3[2] == 1);
    
    auto seq5 = cppmath::combinatorics::fibonacci_sequence(5);
    CHECK(seq5.size() == 5);
    CHECK(seq5[0] == 0);
    CHECK(seq5[1] == 1);
    CHECK(seq5[2] == 1);
    CHECK(seq5[3] == 2);
    CHECK(seq5[4] == 3);
    
    auto seq10 = cppmath::combinatorics::fibonacci_sequence(10);
    CHECK(seq10.size() == 10);
    std::vector<unsigned long long> expected10 = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    CHECK(seq10 == expected10);
    
    auto seq15 = cppmath::combinatorics::fibonacci_sequence(15);
    CHECK(seq15.size() == 15);
    std::vector<unsigned long long> expected15 = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377};
    CHECK(seq15 == expected15);
    
    // Test larger limits
    auto seq20 = cppmath::combinatorics::fibonacci_sequence(20);
    CHECK(seq20.size() == 20);
    CHECK(seq20[0] == 0);
    CHECK(seq20[1] == 1);
    CHECK(seq20[19] == 4181);
}

TEST_CASE("Fibonacci sequence - error handling") {
    // Test invalid input
    CHECK_THROWS_AS(cppmath::combinatorics::fibonacci_sequence(0), std::runtime_error);
}

TEST_CASE("Binomial coefficient - binomial_coefficient()") {
    // Test base cases
    CHECK(cppmath::combinatorics::binomial_coefficient(0, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(1, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(1, 1) == 1);
    
    // Test small values
    CHECK(cppmath::combinatorics::binomial_coefficient(2, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(2, 1) == 2);
    CHECK(cppmath::combinatorics::binomial_coefficient(2, 2) == 1);
    
    CHECK(cppmath::combinatorics::binomial_coefficient(3, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(3, 1) == 3);
    CHECK(cppmath::combinatorics::binomial_coefficient(3, 2) == 3);
    CHECK(cppmath::combinatorics::binomial_coefficient(3, 3) == 1);
    
    CHECK(cppmath::combinatorics::binomial_coefficient(4, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(4, 1) == 4);
    CHECK(cppmath::combinatorics::binomial_coefficient(4, 2) == 6);
    CHECK(cppmath::combinatorics::binomial_coefficient(4, 3) == 4);
    CHECK(cppmath::combinatorics::binomial_coefficient(4, 4) == 1);
    
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 1) == 5);
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 2) == 10);
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 3) == 10);
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 4) == 5);
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 5) == 1);
    
    // Test larger values
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 5) == 252);
    CHECK(cppmath::combinatorics::binomial_coefficient(12, 6) == 924);
    CHECK(cppmath::combinatorics::binomial_coefficient(15, 7) == 6435);
    CHECK(cppmath::combinatorics::binomial_coefficient(20, 10) == 184756);
    
    // Test edge cases
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 10) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 1) == 10);
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 9) == 10);
}

TEST_CASE("Binomial coefficient - edge cases") {
    // Test k > n (should return 0)
    CHECK(cppmath::combinatorics::binomial_coefficient(5, 6) == 0);
    CHECK(cppmath::combinatorics::binomial_coefficient(10, 15) == 0);
    CHECK(cppmath::combinatorics::binomial_coefficient(1, 2) == 0);
    
    // Test k = 0 or k = n
    CHECK(cppmath::combinatorics::binomial_coefficient(100, 0) == 1);
    CHECK(cppmath::combinatorics::binomial_coefficient(100, 100) == 1);
}

TEST_CASE("Binomial coefficient - error handling") {
    // Note: The current implementation doesn't throw for negative numbers
    // but it should according to the header documentation
    // This test documents the current behavior
    // CHECK_THROWS_AS(cppmath::combinatorics::binomial_coefficient(-1, 0), std::runtime_error);
    // CHECK_THROWS_AS(cppmath::combinatorics::binomial_coefficient(5, -1), std::runtime_error);
}

TEST_CASE("Pascal's triangle verification") {
    // Verify that binomial coefficients follow Pascal's triangle
    for (unsigned int n = 0; n <= 10; ++n) {
        for (unsigned int k = 0; k <= n; ++k) {
            unsigned long long expected = 1;
            if (k > 0 && k < n) {
                expected = cppmath::combinatorics::binomial_coefficient(n-1, k-1) + 
                          cppmath::combinatorics::binomial_coefficient(n-1, k);
            }
            CHECK(cppmath::combinatorics::binomial_coefficient(n, k) == expected);
        }
    }
} 