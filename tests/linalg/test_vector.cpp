#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/linalg/vector.hpp"
#include <vector>

TEST_CASE("Dot product") {
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {4, 5, 6};
    
    // Basic dot product
    CHECK(cppmath::linalg::dot_product(a, b) == 32); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    
    // Test with doubles
    std::vector<double> c = {1.5, 2.5};
    std::vector<double> d = {2.0, 3.0};
    CHECK(cppmath::linalg::dot_product(c, d) == 1.5*2.0 + 2.5*3.0);
    
    // Test dimension mismatch
    std::vector<int> e = {1, 2};
    CHECK_THROWS_AS(cppmath::linalg::dot_product(a, e), std::invalid_argument);
}

TEST_CASE("Cross product") {
    std::vector<int> a = {1, 0, 0}; // i unit vector
    std::vector<int> b = {0, 1, 0}; // j unit vector
    
    auto result = cppmath::linalg::cross_product(a, b);
    CHECK(result[0] == 0);
    CHECK(result[1] == 0);
    CHECK(result[2] == 1); // Should be k unit vector
    
    // Test another case
    std::vector<double> c = {1, 2, 3};
    std::vector<double> d = {4, 5, 6};
    auto result2 = cppmath::linalg::cross_product(c, d);
    CHECK(result2[0] == 2*6 - 3*5); // -3
    CHECK(result2[1] == 3*4 - 1*6); // 6
    CHECK(result2[2] == 1*5 - 2*4); // -3
    
    // Test non-3D vectors
    std::vector<int> e = {1, 2};
    CHECK_THROWS_AS(cppmath::linalg::cross_product(a, e), std::invalid_argument);
}

TEST_CASE("Magnitude") {
    std::vector<int> a = {3, 4}; // 3-4-5 triangle
    CHECK(std::abs(cppmath::linalg::magnitude(a) - 5.0) < 1e-10);
    
    std::vector<double> b = {1, 1, 1}; // sqrt(3)
    CHECK(std::abs(cppmath::linalg::magnitude(b) - std::sqrt(3.0)) < 1e-10);
    
    std::vector<int> c = {0, 0, 0}; // zero vector
    CHECK(cppmath::linalg::magnitude(c) == 0.0);
}

TEST_CASE("Normalize") {
    std::vector<int> a = {3, 4}; // 3-4-5 triangle
    auto normalized = cppmath::linalg::normalize(a);
    CHECK(std::abs(cppmath::linalg::magnitude(normalized) - 1.0) < 1e-10);
    CHECK(std::abs(normalized[0] - 0.6) < 1e-10);
    CHECK(std::abs(normalized[1] - 0.8) < 1e-10);
    
    // Test zero vector (should throw)
    std::vector<int> b = {0, 0, 0};
    CHECK_THROWS_AS(cppmath::linalg::normalize(b), std::invalid_argument);
} 