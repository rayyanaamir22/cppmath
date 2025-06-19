#include <iostream>
#include "cppmath/linalg.hpp"

int main() {
    cppmath::linalg::Matrix<double> m(2, 2);
    m(0, 0) = 1.0;
    m(0, 1) = 2.0;
    m(1, 0) = 3.0;
    m(1, 1) = 4.0;
    std::cout << "Matrix(0,0): " << m(0, 0) << std::endl;
    std::cout << "Matrix(1,1): " << m(1, 1) << std::endl;
    return 0;
}
