#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/linalg.hpp"

TEST_CASE("Matrix basic construction and access") {
    cppmath::linalg::Matrix<double> m(2, 2);
    m(0, 0) = 1.0;
    m(0, 1) = 2.0;
    m(1, 0) = 3.0;
    m(1, 1) = 4.0;
    CHECK(m(0, 0) == 1.0);
    CHECK(m(0, 1) == 2.0);
    CHECK(m(1, 0) == 3.0);
    CHECK(m(1, 1) == 4.0);
    CHECK(m.rows() == 2);
    CHECK(m.cols() == 2);
}
