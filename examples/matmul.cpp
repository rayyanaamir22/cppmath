#include <iostream>
#include "cppmath/linalg.hpp"

using cppmath::linalg::Matrix;

void print_matrix(const Matrix<double>& mat) {
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            std::cout << mat(i, j) << ' ';
        }
        std::cout << '\n';
    }
}

int main() {
    // Valid multiplication
    Matrix<double> A(2, 3);
    Matrix<double> B(3, 2);
    // Fill A
    double val = 1.0;
    for (size_t i = 0; i < A.rows(); ++i)
        for (size_t j = 0; j < A.cols(); ++j)
            A(i, j) = val++;
    // Fill B
    val = 1.0;
    for (size_t i = 0; i < B.rows(); ++i)
        for (size_t j = 0; j < B.cols(); ++j)
            B(i, j) = val++;
    std::cout << "Matrix A:\n";
    print_matrix(A);
    std::cout << "Matrix B:\n";
    print_matrix(B);
    Matrix<double> C = A * B;
    std::cout << "A * B = \n";
    print_matrix(C);

    // Invalid multiplication (should log error)
    Matrix<double> D(2, 2);
    Matrix<double> E = A * D;
    std::cout << "A * D (invalid) = \n";
    print_matrix(E);

    return 0;
} 