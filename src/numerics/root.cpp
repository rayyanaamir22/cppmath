/**
 * @file root.cpp
 * @brief Numerical solver functions implementation.
 */
#include "cppmath/numerics/root.hpp"
#include <stdexcept>

namespace cppmath {
namespace numerics {

double newton_sqrt(double x, double tolerance, int max_iterations) {
    if (x < 0) {
        // TODO: support complex numbers when added; it'll just be `\pm i * sqrt(x)`
        throw std::invalid_argument("Cannot compute square root of negative number");
    }
    if (x == 0) return 0;
    
    // Initial guess: x/2
    double guess = x / 2.0;
    
    // improve for max_iterations
    for (int i = 0; i < max_iterations; ++i) {
        double next_guess = 0.5 * (guess + x / guess);

        // if we enter the tolerance range, just return this
        if (std::abs(next_guess - guess) < tolerance) {
            return next_guess;
        }
        guess = next_guess;
    }
    
    // Return best approximation if max iterations reached (never reached tolerance)
    return guess;
}

double newton_cbrt(double x, double tolerance, int max_iterations) {
    if (x == 0) return 0;
    
    // Initial guess: x/3
    double guess = x / 3.0;
    
    for (int i = 0; i < max_iterations; ++i) {
        double next_guess = (2.0 * guess + x / (guess * guess)) / 3.0;
        if (std::abs(next_guess - guess) < tolerance) {
            return next_guess;
        }
        guess = next_guess;
    }
    
    return guess; // Return best approximation if max iterations reached
}

} // namespace numerics
} // namespace cppmath 