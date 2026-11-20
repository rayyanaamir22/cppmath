#include <iostream>
#include "cppmath/linalg/matrix.hpp"

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
    Matrix<double> A(3, 3);
    Matrix<double> B(3, 3);

    // Fill A
    double val = 1.0;
    for (size_t i = 0; i < A.rows(); ++i)
        for (size_t j = 0; j < A.cols(); ++j)
            A(i, j) = val++;

    // Fill B (identity matrix)
    for (size_t i = 0; i < B.rows(); ++i)
        B(i, i) = 1.0;
    
    std::cout << "Matrix A:\n";
    print_matrix(A);
    std::cout << "Matrix B:\n";
    print_matrix(B);
    Matrix<double> C = A * B;
    std::cout << "A * B = \n";
    print_matrix(C);

    // Invalid multiplication: incompatible dimensions throw std::invalid_argument
    Matrix<double> D(2, 2);
    try {
        Matrix<double> E = A * D;
        print_matrix(E);
    } catch (const std::invalid_argument& e) {
        std::cout << "A * D (invalid): " << e.what() << "\n";
    }

    return 0;
} 