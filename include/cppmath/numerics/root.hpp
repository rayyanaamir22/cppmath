#pragma once
#include <cmath>

/**
 * @brief Numerical methods (solvers, interpolation, etc.)
 */
namespace cppmath {
namespace numerics {
// Add numerical method declarations here

/**
 * @brief Computes the square root using Newton's method.
 * @param x The number to find the square root of.
 * @param tolerance The tolerance for convergence (default: 1e-10).
 * @param max_iterations Maximum number of iterations (default: 100).
 * @return The square root of x.
 */
double newton_sqrt(double x, double tolerance = 1e-10, int max_iterations = 100);

/**
 * @brief Computes the cube root using Newton's method.
 * @param x The number to find the cube root of.
 * @param tolerance The tolerance for convergence (default: 1e-10).
 * @param max_iterations Maximum number of iterations (default: 100).
 * @return The cube root of x.
 */
double newton_cbrt(double x, double tolerance = 1e-10, int max_iterations = 100);
}
}
