#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/linalg/matrix.hpp"

TEST_CASE("Matrix construction and access") {
    cppmath::linalg::Matrix<int> m(2, 3);
    m(0, 0) = 1;
    m(0, 1) = 2;
    m(0, 2) = 3;
    m(1, 0) = 4;
    m(1, 1) = 5;
    m(1, 2) = 6;
    CHECK(m(0, 0) == 1);
    CHECK(m(0, 1) == 2);
    CHECK(m(0, 2) == 3);
    CHECK(m(1, 0) == 4);
    CHECK(m(1, 1) == 5);
    CHECK(m(1, 2) == 6);
    CHECK(m.rows() == 2);
    CHECK(m.cols() == 3);
}

TEST_CASE("Matrix multiplication") {
    cppmath::linalg::Matrix<int> a(2, 3);
    cppmath::linalg::Matrix<int> b(3, 2);
    // Fill a
    a(0, 0) = 1; a(0, 1) = 2; a(0, 2) = 3;
    a(1, 0) = 4; a(1, 1) = 5; a(1, 2) = 6;
    // Fill b
    b(0, 0) = 7;  b(0, 1) = 8;
    b(1, 0) = 9;  b(1, 1) = 10;
    b(2, 0) = 11; b(2, 1) = 12;
    // Expected result:
    // [ 58  64 ]
    // [139 154]
    auto c = a * b;
    CHECK(c.rows() == 2);
    CHECK(c.cols() == 2);
    CHECK(c(0, 0) == 58);
    CHECK(c(0, 1) == 64);
    CHECK(c(1, 0) == 139);
    CHECK(c(1, 1) == 154);
} 