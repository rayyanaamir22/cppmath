/**
 * @file combo.cpp
 * @brief Combinatorics functions implementation.
 */
#include "cppmath/combinatorics/combo.hpp"
#include <stdexcept>

namespace cppmath {
namespace combinatorics {

unsigned long long fibonacci(unsigned int n) {
    // memoized dp approach
    if (n < 1) {
        throw std::runtime_error("n must be at least 1");
    }
    // use static vector for memoization
    static std::vector<unsigned long long> memo = {0, 0, 1}; // memo[1]=0, memo[2]=1, memo[3]=1

    // base cases
    if (n < memo.size()) {
        return memo[n];
    }

    // compute and memoize missing values up to n
    for (unsigned int i = memo.size(); i <= n; ++i) {
        memo.push_back(memo[i-1] + memo[i-2]);
    }
    return memo[n];
}

unsigned long long factorial(unsigned int n) {
    if (n < 0) {
        throw std::runtime_error("factorial of negative numbers not supported");
    }
    else if (n == 0) {
        return 1;
    }
    return n * factorial(n-1);
}

std::vector<unsigned long long> fibonacci_sequence(unsigned long long limit) {
    if (limit < 1) {
        throw std::runtime_error("limit must be at least 1");
    }
    
    std::vector<unsigned long long> sequence;
    
    // Generate Fibonacci numbers up to the limit
    unsigned long long a = 0, b = 1;
    unsigned int count = 0;
    
    while (count < limit) {
        sequence.push_back(a);
        unsigned long long temp = a + b;
        a = b;
        b = temp;
        count++;
    }
    
    return sequence;
}

unsigned long long binomial_coefficient(unsigned int n, unsigned int k) {
    if (n < 0) {
        throw std::runtime_error("n, k cannot be zero");
    }
    else if (k < 0 || k > n) {
        return 0;
    }
    else if (k == 0 || n == 1 || n == k) {
        return 1;
    }
    return factorial(n) / ( factorial(k) * (factorial(n-k)) );
}

} // namespace combinatorics
} // namespace cppmath 