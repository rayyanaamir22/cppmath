#pragma once
#include <vector>

/**
 * @brief Combinatorics functions (permutations, combinations, etc.)
 */
namespace cppmath {
namespace combinatorics {

/**
 * @brief Computes the nth Fibonacci number.
 * @param n The index of the Fibonacci number to compute.
 * @return The nth Fibonacci number.
 * @throws std::invalid_argument if n is negative.
 */
unsigned long long fibonacci(unsigned int n);

/**
 * @brief Computes the factorial of n.
 * @param n The number to compute factorial for.
 * @return n! (factorial of n).
 * @throws std::invalid_argument if n is negative.
 */
unsigned long long factorial(unsigned int n);

/**
 * @brief Generates Fibonacci numbers up to a limit.
 * @param limit The maximum value to generate Fibonacci numbers up to.
 * @return Vector of Fibonacci numbers less than or equal to limit.
 */
std::vector<unsigned long long> fibonacci_sequence(unsigned long long limit);

/**
 * @brief Computes binomial coefficient C(n,k).
 * @param n The total number of items.
 * @param k The number of items to choose.
 * @return C(n,k) = n!/(k!(n-k)!).
 * @throws std::invalid_argument if k > n or if either is negative.
 */
unsigned long long binomial_coefficient(unsigned int n, unsigned int k);

// Add combinatorics function declarations here
}
}
