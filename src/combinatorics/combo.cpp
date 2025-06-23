/**
 * @file combo.cpp
 * @brief Combinatorics functions implementation.
 */
#include "cppmath/combinatorics/combo.hpp"
#include <stdexcept>

namespace cppmath {
namespace combinatorics {

unsigned long long fibonacci(unsigned int n) {
    // TODO: replace w memoization
    // basic recursion
    if (n == 1) {
        return 0;
    } else if (n < 4) {
        return 1;
    }
    else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

unsigned long long factorial(unsigned int n) {
    if (n < 0) {
        std::runtime_error("factorial of negative numbers not supported");
    }
    else if (n == 0) {
        return 1;
    }
    return n * factorial(n-1);
}

std::vector<unsigned long long> fibonacci_sequence(unsigned long long limit) {
    // TODO: Implement Fibonacci sequence generation
    throw std::runtime_error("fibonacci_sequence() not yet implemented");
}

unsigned long long binomial_coefficient(unsigned int n, unsigned int k) {
    if (n < 0) {
        throw std::runtime_error("n, k cannot be zero");
    }
    else if (k < 0 || k > n) {
        return 0;
    }
    else if (k == 0 || n == 1) {
        return 1;
    }
    return factorial(n) / ( factorial(k) * (factorial(n-k)) );
}

} // namespace combinatorics
} // namespace cppmath 