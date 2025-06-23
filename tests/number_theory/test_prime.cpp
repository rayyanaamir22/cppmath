#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/number_theory/prime.hpp"

TEST_CASE("Prime testing") {
    // TODO: Add tests when is_prime() is implemented
    // CHECK(cppmath::number_theory::is_prime(2) == true);
    // CHECK(cppmath::number_theory::is_prime(3) == true);
    // CHECK(cppmath::number_theory::is_prime(4) == false);
    // CHECK(cppmath::number_theory::is_prime(17) == true);
}

TEST_CASE("Prime generation") {
    // TODO: Add tests when generate_primes() is implemented
    // auto primes = cppmath::number_theory::generate_primes(20);
    // CHECK(primes.size() == 8); // 2, 3, 5, 7, 11, 13, 17, 19
}

TEST_CASE("Next prime") {
    // TODO: Add tests when next_prime() is implemented
    // CHECK(cppmath::number_theory::next_prime(10) == 11);
    // CHECK(cppmath::number_theory::next_prime(17) == 19);
}

TEST_CASE("GCD") {
    // TODO: Add tests when gcd() is implemented
    // CHECK(cppmath::number_theory::gcd(48, 18) == 6);
    // CHECK(cppmath::number_theory::gcd(54, 24) == 6);
    // CHECK(cppmath::number_theory::gcd(7, 13) == 1); // coprime
}

TEST_CASE("LCM") {
    // TODO: Add tests when lcm() is implemented
    // CHECK(cppmath::number_theory::lcm(12, 18) == 36);
    // CHECK(cppmath::number_theory::lcm(8, 12) == 24);
}

TEST_CASE("GCD and LCM for multiple numbers") {
    // TODO: Add tests when gcd_lcm() is implemented
    // std::vector<unsigned long long> nums = {12, 18, 24};
    // auto result = cppmath::number_theory::gcd_lcm(nums);
    // CHECK(result.first == 6);  // GCD
    // CHECK(result.second == 72); // LCM
}

TEST_CASE("Prime factorization") {
    // TODO: Add tests when prime_factorization() is implemented
    // auto factors = cppmath::number_theory::prime_factorization(84);
    // CHECK(factors.size() == 4); // 2, 2, 3, 7
} 