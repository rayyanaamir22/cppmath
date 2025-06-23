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

// Helper: Create identity matrix of size n
cppmath::linalg::Matrix<int> identity_matrix(size_t n) {
    cppmath::linalg::Matrix<int> m(n, n);
    for (size_t i = 0; i < n; ++i) m(i, i) = 1;
    return m;
}

// Helper: Create zero matrix of size rows x cols
cppmath::linalg::Matrix<int> zero_matrix(size_t rows, size_t cols) {
    return cppmath::linalg::Matrix<int>(rows, cols); // default-initialized to zero
}

TEST_CASE("Identity matrix properties") {
    for (size_t n : {1, 2, 5, 10}) {
        auto I = identity_matrix(n);
        // Check diagonal is 1, off-diagonal is 0
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) CHECK(I(i, j) == 1);
                else CHECK(I(i, j) == 0);
            }
        }
        // I * I = I
        auto II = I * I;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                CHECK(II(i, j) == I(i, j));
    }
}

TEST_CASE("Zero matrix properties") {
    for (size_t rows : {1, 3}) {
        for (size_t cols : {1, 4}) {
            auto Z = zero_matrix(rows, cols);
            for (size_t i = 0; i < rows; ++i)
                for (size_t j = 0; j < cols; ++j)
                    CHECK(Z(i, j) == 0);
        }
    }
}

TEST_CASE("Matrix multiplication dimension mismatch") {
    cppmath::linalg::Matrix<int> a(2, 3);
    cppmath::linalg::Matrix<int> b(4, 2); // incompatible
    auto c = a * b;
    CHECK(c.rows() == 0);
    CHECK(c.cols() == 0);
}

TEST_CASE("Determinant computation (not implemented)") {
    // Placeholder: When det() is implemented, add tests like:
    // cppmath::linalg::Matrix<int> m(2,2);
    // m(0,0)=1; m(0,1)=2; m(1,0)=3; m(1,1)=4;
    // CHECK(m.det() == -2);
    CHECK(true); // Remove when implemented
} 