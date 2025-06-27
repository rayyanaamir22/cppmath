#include "cppmath/linalg/vector.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace cppmath::linalg;

int main() {
    const size_t N = 1000000;
    Vector a(std::vector<double>(N, 1.0));
    Vector b(std::vector<double>(N, 2.0));

    // Non-SIMD addition
    auto t1 = std::chrono::high_resolution_clock::now();
    Vector c = a.vector_add(b, false);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_non_simd = t2 - t1;
    std::cout << "Non-SIMD addition: " << elapsed_non_simd.count() << " s\n";

    // SIMD addition
    t1 = std::chrono::high_resolution_clock::now();
    Vector c_simd = a.vector_add(b, true);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_simd = t2 - t1;
    std::cout << "SIMD addition:    " << elapsed_simd.count() << " s\n";

    // Non-SIMD subtraction
    t1 = std::chrono::high_resolution_clock::now();
    Vector d = a.vector_sub(b, false);
    t2 = std::chrono::high_resolution_clock::now();
    elapsed_non_simd = t2 - t1;
    std::cout << "Non-SIMD subtraction: " << elapsed_non_simd.count() << " s\n";

    // SIMD subtraction
    t1 = std::chrono::high_resolution_clock::now();
    Vector d_simd = a.vector_sub(b, true);
    t2 = std::chrono::high_resolution_clock::now();
    elapsed_simd = t2 - t1;
    std::cout << "SIMD subtraction:    " << elapsed_simd.count() << " s\n";

    // Prevent optimization
    std::cout << "c[0]=" << c[0] << ", c_simd[0]=" << c_simd[0] << ", d[0]=" << d[0] << ", d_simd[0]=" << d_simd[0] << std::endl;
    return 0;
} 