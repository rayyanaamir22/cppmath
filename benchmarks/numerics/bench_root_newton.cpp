#include "cppmath/numerics/root.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main() {
    constexpr size_t N = 1000000;
    constexpr double tolerance = 1e-10;
    constexpr int max_iterations = 100;
    std::vector<double> data(N);
    std::vector<double> results_newton(N);
    std::vector<double> results_std(N);

    // Generate random positive doubles
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(1.0, 10000.0);
    for (size_t i = 0; i < N; ++i) {
        data[i] = dist(rng);
    }

    // Benchmark newton_sqrt
    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        results_newton[i] = cppmath::numerics::newton_sqrt(data[i], tolerance, max_iterations);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_newton = t2 - t1;
    std::cout << "cppmath::numerics::newton_sqrt: " << elapsed_newton.count() << " s\n";

    // Benchmark std::sqrt
    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        results_std[i] = std::sqrt(data[i]);
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_std = t2 - t1;
    std::cout << "std::sqrt: " << elapsed_std.count() << " s\n";

    // Compute average absolute error
    double avg_error = 0.0;
    for (size_t i = 0; i < N; ++i) {
        avg_error += std::abs(results_newton[i] - results_std[i]);
    }
    avg_error /= N;
    std::cout << "Average absolute error: " << avg_error << std::endl;

    // Prevent optimization
    std::cout << "Sample: \n newton_sqrt(" << data[0] << ") = " << results_newton[0]
              << "\n std::sqrt(" << data[0] << ") = " << results_std[0] << std::endl;
    return 0;
}
