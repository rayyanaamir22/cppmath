/**
 * @file prime.cpp
 * @brief Prime number related functions implementation.
 */
#include "cppmath/number_theory/prime.hpp"
#include "cppmath/numerics/newton_sqrt.hpp"
#include <cmath>
#include <stdexcept>

namespace cppmath {
namespace number_theory {

bool is_prime(unsigned long long n) {
    // sieve of eratosthenes approach

    // sqrt of n, rounded down
    unsigned long long sqrt_n = static_cast<unsigned long long>(std::floor(numerics::newton_sqrt(n)));

    // check if anything from 2 to sqrt_n (floored) divides n
    for (size_t i = 2; i < sqrt_n; i++) {
        if (n % i == 0) {
            return false; // something divided it
        }
    } return true;
}

std::vector<unsigned long long> generate_primes(unsigned long long limit) {
    // naive algo

    if (limit < 1) {
        throw std::runtime_error("limit must be greater than 0");
    }

    // list of primes up to limit
    static std::vector<unsigned long long> primes;

    if (limit < 4) {
        // i.e. limit = 1, 2, 3
        for (size_t i = 1; i < limit; i++) {
            primes.push_back(i);
        } return primes;
    }

    // even is never prime (except 2, which we handled in the case above)
    if (n % 2 == 0) {
        return false
    }
    // otherwise, sieve of eratosthenes
    else {
        // skip 4 since it's not prime
        for (size_t i = 5; i < limit; i++) {
            if (is_prime(i)) {
                primes.push_back(i);
            }
        }
        return primes;
    }

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
    // TODO: unfinished
    if (n < 1) {
        throw std::runtime_error("limit must be greater than 0");
    }

    // list of prime factors
    static std::vector<unsigned long long> prime_factors;

    // n itself is prime, so it's the only factor lol
    else if (is_prime(n)) {
        // return it in the vector still
        prime_factors.push_back(n); return prime_factors;
    }

    else {
        // even case
        if (n % 2 == 0) {
            // reduce even numbers (repeatedly divide by 2)
            while (n % 2 == 0) {
                n = n / 2;
                prime_factors.push_back(2);
            }
            // get prime factors of remaining odd number
            std::vector<unsigned long long> odd_remainder_prime_factors = prime_factorization(n);
            // concatenate them
            prime_factors.insert( 
                prime_factors.end(), 
                odd_remainder_prime_factors.begin(), 
                odd_remainder_prime_factors.end() 
            );
            return prime_factors;
        }
        // odd case
        // TODO
        throw std::runtime_error("case not implemented yet :p");
    }
}

} // namespace number_theory
} // namespace cppmath 