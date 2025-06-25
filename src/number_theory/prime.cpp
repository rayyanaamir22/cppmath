/**
 * @file prime.cpp
 * @brief Prime number related functions implementation.
 */
#include "cppmath/number_theory/prime.hpp"
#include "cppmath/numerics/root.hpp"
#include <cmath>
#include <stdexcept>

namespace cppmath {
namespace number_theory {

bool is_prime(unsigned long long n) {
    if (n == 2) return true;
    if (n < 2 || n % 2 == 0) return false;

    // sieve of eratosthenes approach

    // sqrt of n, rounded down
    unsigned long long sqrt_n = static_cast<unsigned long long>(std::floor(cppmath::numerics::newton_sqrt(n)));

    // check if anything odd from 3 to sqrt_n (floored) divides n
    // at this point, we know n isn't even
    for (unsigned long long i = 3; i <= sqrt_n; i += 2) {
        if (n % i == 0) {
            return false; // something divided it
        }
    } return true;
}

std::vector<unsigned long long> generate_primes(unsigned long long limit) {
    if (limit < 2) {
        return {};
    }
    std::vector<unsigned long long> primes;
    primes.push_back(2);
    for (unsigned long long i = 3; i <= limit; i += 2) {
        if (is_prime(i)) {
            primes.push_back(i);
        }
    }
    return primes;
}

unsigned long long next_prime(unsigned long long n) {
    // TODO: Implement next prime finding
    throw std::runtime_error("next_prime() not yet implemented");
}

unsigned long long gcd(unsigned long long a, unsigned long long b) {
    // TODO: Implement Euclidean algorithm for GCD
    throw std::runtime_error("gcd() not yet implemented");
}

unsigned long long lcm(unsigned long long a, unsigned long long b) {
    // TODO: Implement LCM computation
    throw std::runtime_error("lcm() not yet implemented");
}

std::pair<unsigned long long, unsigned long long> gcd_lcm(const std::vector<unsigned long long>& numbers) {
    // TODO: ***AI generated*** prototype
    if (numbers.empty()) {
        throw std::runtime_error("Input vector must not be empty");
    }
    unsigned long long current_gcd = numbers[0];
    unsigned long long current_lcm = numbers[0];
    for (size_t i = 1; i < numbers.size(); ++i) {
        current_gcd = gcd(current_gcd, numbers[i]);
        current_lcm = lcm(current_lcm, numbers[i]);
    }
    return std::make_pair(current_gcd, current_lcm);
}

std::vector<unsigned long long> prime_factorization(unsigned long long n) {
    if (n < 1) {
        throw std::runtime_error("n must be greater than 0");
    }
    std::vector<unsigned long long> factors;
    // Handle 2 separately
    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }
    // Handle odd factors
    for (unsigned long long i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }
    // If n is a prime number > 2
    if (n > 1) {
        factors.push_back(n);
    }
    return factors;
}

} // namespace number_theory
} // namespace cppmath 