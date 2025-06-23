#pragma once
#include <vector>

/**
 * @brief Number theory functions (gcd, primes, etc.)
 */
namespace cppmath {
namespace number_theory {

/**
 * @brief Tests if a number is prime.
 * @param n The number to test.
 * @return true if n is prime, false otherwise.
 */
bool is_prime(unsigned long long n);

/**
 * @brief Generates all prime numbers up to a limit using Sieve of Eratosthenes.
 * @param limit The upper bound for prime generation.
 * @return Vector of all prime numbers less than or equal to limit.
 */
std::vector<unsigned long long> generate_primes(unsigned long long limit);

/**
 * @brief Finds the next prime number greater than n.
 * @param n The starting number.
 * @return The next prime number greater than n.
 */
unsigned long long next_prime(unsigned long long n);

/**
 * @brief Computes the greatest common divisor of two numbers using Euclidean algorithm.
 * @param a First number.
 * @param b Second number.
 * @return GCD of a and b.
 */
unsigned long long gcd(unsigned long long a, unsigned long long b);

/**
 * @brief Computes the least common multiple of two numbers.
 * @param a First number.
 * @param b Second number.
 * @return LCM of a and b.
 */
unsigned long long lcm(unsigned long long a, unsigned long long b);

/**
 * @brief Computes GCD and LCM of multiple numbers.
 * @param numbers Vector of numbers.
 * @return Pair containing (GCD, LCM) of all numbers.
 */
std::pair<unsigned long long, unsigned long long> gcd_lcm(const std::vector<unsigned long long>& numbers);

/**
 * @brief Computes the prime factorization of a number.
 * @param n The number to factorize.
 * @return Vector of prime factors (with multiplicity).
 */
std::vector<unsigned long long> prime_factorization(unsigned long long n);

} // namespace number_theory
} // namespace cppmath
