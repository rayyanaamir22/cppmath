#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/linalg/vector.hpp"
#include <vector>
#include <stdexcept>

using namespace cppmath::linalg;

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

TEST_CASE("Vector Construction") {
    SUBCASE("Default constructor") {
        Vector v;
        CHECK(v.size() == 0);
    }
    
    SUBCASE("Size constructor") {
        Vector v(3);
        CHECK(v.size() == 3);
        CHECK(v[0] == 0.0);
        CHECK(v[1] == 0.0);
        CHECK(v[2] == 0.0);
    }
    
    SUBCASE("Vector constructor") {
        std::vector<double> comps = {1.0, 2.0, 3.0};
        Vector v(comps);
        CHECK(v.size() == 3);
        CHECK(v[0] == 1.0);
        CHECK(v[1] == 2.0);
        CHECK(v[2] == 3.0);
    }
    
    SUBCASE("Initializer list constructor") {
        Vector v = {1.0, 2.0, 3.0};
        CHECK(v.size() == 3);
        CHECK(v[0] == 1.0);
        CHECK(v[1] == 2.0);
        CHECK(v[2] == 3.0);
    }
    
    SUBCASE("Copy constructor") {
        Vector v1 = {1.0, 2.0, 3.0};
        Vector v2(v1);
        CHECK(v2.size() == 3);
        CHECK(v2[0] == 1.0);
        CHECK(v2[1] == 2.0);
        CHECK(v2[2] == 3.0);
    }
}

TEST_CASE("Vector Assignment") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2;
    v2 = v1;
    CHECK(v2.size() == 3);
    CHECK(v2[0] == 1.0);
    CHECK(v2[1] == 2.0);
    CHECK(v2[2] == 3.0);
}

TEST_CASE("Vector Access") {
    Vector v = {1.0, 2.0, 3.0};
    CHECK(v[0] == 1.0);
    CHECK(v[1] == 2.0);
    CHECK(v[2] == 3.0);
    
    v[0] = 5.0;
    CHECK(v[0] == 5.0);
    
    SUBCASE("Out of range access") {
        CHECK_THROWS_AS(v[3], std::out_of_range);
    }
}

TEST_CASE("Vector Addition") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    Vector result = v1 + v2;
    
    CHECK(result.size() == 3);
    CHECK(result[0] == 5.0);
    CHECK(result[1] == 7.0);
    CHECK(result[2] == 9.0);
    
    SUBCASE("Different dimensions") {
        Vector v3 = {1.0, 2.0};
        CHECK_THROWS_AS(v1 + v3, std::invalid_argument);
    }
}

TEST_CASE("Vector Subtraction") {
    Vector v1 = {5.0, 7.0, 9.0};
    Vector v2 = {1.0, 2.0, 3.0};
    Vector result = v1 - v2;
    
    CHECK(result.size() == 3);
    CHECK(result[0] == 4.0);
    CHECK(result[1] == 5.0);
    CHECK(result[2] == 6.0);
}

TEST_CASE("Vector Scalar Multiplication") {
    Vector v = {1.0, 2.0, 3.0};
    Vector result = v * 2.0;
    
    CHECK(result.size() == 3);
    CHECK(result[0] == 2.0);
    CHECK(result[1] == 4.0);
    CHECK(result[2] == 6.0);
    
    // Test friend function
    Vector result2 = 3.0 * v;
    CHECK(result2[0] == 3.0);
    CHECK(result2[1] == 6.0);
    CHECK(result2[2] == 9.0);
}

TEST_CASE("Vector Scalar Division") {
    Vector v = {6.0, 8.0, 10.0};
    Vector result = v / 2.0;
    
    CHECK(result.size() == 3);
    CHECK(result[0] == 3.0);
    CHECK(result[1] == 4.0);
    CHECK(result[2] == 5.0);
    
    SUBCASE("Division by zero") {
        CHECK_THROWS_AS(v / 0.0, std::invalid_argument);
    }
}

TEST_CASE("Vector Compound Assignment") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    
    v1 += v2;
    CHECK(v1[0] == 5.0);
    CHECK(v1[1] == 7.0);
    CHECK(v1[2] == 9.0);
    
    v1 -= v2;
    CHECK(v1[0] == 1.0);
    CHECK(v1[1] == 2.0);
    CHECK(v1[2] == 3.0);
    
    v1 *= 2.0;
    CHECK(v1[0] == 2.0);
    CHECK(v1[1] == 4.0);
    CHECK(v1[2] == 6.0);
    
    v1 /= 2.0;
    CHECK(v1[0] == 1.0);
    CHECK(v1[1] == 2.0);
    CHECK(v1[2] == 3.0);
}

TEST_CASE("Vector Comparison") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {1.0, 2.0, 3.0};
    Vector v3 = {1.0, 2.0, 4.0};
    Vector v4 = {1.0, 2.0};
    
    CHECK(v1 == v2);
    CHECK(v1 != v3);
    CHECK(v1 != v4);
}

TEST_CASE("Vector Dot Product") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    double result = v1.dot_product(v2);
    
    CHECK(result == 32.0); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    
    SUBCASE("Different dimensions") {
        Vector v3 = {1.0, 2.0};
        CHECK_THROWS_AS(v1.dot_product(v3), std::invalid_argument);
    }
}

TEST_CASE("Vector Cross Product") {
    Vector v1 = {1.0, 0.0, 0.0};
    Vector v2 = {0.0, 1.0, 0.0};
    Vector result = v1.cross_product(v2);
    
    CHECK(result.size() == 3);
    CHECK(result[0] == 0.0);
    CHECK(result[1] == 0.0);
    CHECK(result[2] == 1.0);
    
    SUBCASE("Non-3D vectors") {
        Vector v3 = {1.0, 2.0};
        CHECK_THROWS_AS(v1.cross_product(v3), std::invalid_argument);
    }
}

TEST_CASE("Vector Magnitude") {
    Vector v = {3.0, 4.0, 0.0};
    double mag = v.magnitude();
    
    CHECK(mag == 5.0); // sqrt(3^2 + 4^2 + 0^2) = sqrt(9 + 16 + 0) = sqrt(25) = 5
}

TEST_CASE("Vector Normalization") {
    Vector v = {3.0, 4.0, 0.0};
    Vector normalized = v.normalize();
    
    CHECK(normalized.magnitude() == doctest::Approx(1.0));
    CHECK(normalized[0] == doctest::Approx(0.6));
    CHECK(normalized[1] == doctest::Approx(0.8));
    CHECK(normalized[2] == doctest::Approx(0.0));
    
    SUBCASE("Zero vector") {
        Vector zero = {0.0, 0.0, 0.0};
        CHECK_THROWS_AS(zero.normalize(), std::invalid_argument);
    }
}

TEST_CASE("Vector Utility Methods") {
    Vector v = {0.0, 0.0, 0.0};
    CHECK(v.is_zero());
    
    v[0] = 1.0;
    CHECK(!v.is_zero());
    
    v.resize(5);
    CHECK(v.size() == 5);
    CHECK(v[4] == 0.0);
    
    v.clear();
    CHECK(v.size() == 0);
}

TEST_CASE("VectorElement Interface") {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    
    // Test add method
    auto* result = v1.add(v2);
    Vector* vec_result = dynamic_cast<Vector*>(result);
    CHECK(vec_result != nullptr);
    CHECK((*vec_result)[0] == 5.0);
    CHECK((*vec_result)[1] == 7.0);
    CHECK((*vec_result)[2] == 9.0);
    delete result;
    
    // Test scalar_multiply method
    auto* scaled = v1.scalar_multiply(2.0);
    Vector* vec_scaled = dynamic_cast<Vector*>(scaled);
    CHECK(vec_scaled != nullptr);
    CHECK((*vec_scaled)[0] == 2.0);
    CHECK((*vec_scaled)[1] == 4.0);
    CHECK((*vec_scaled)[2] == 6.0);
    delete scaled;
} 