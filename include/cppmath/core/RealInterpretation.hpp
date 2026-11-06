#pragma once
#include "Evaluate.hpp"

namespace cppmath {
namespace core {

/**
 * @brief The standard operators interpreted on doubles.
 *
 * Bad domains throw std::invalid_argument rather than returning NaN/inf:
 * division by zero, log of x <= 0, sqrt of x < 0, and pow with an undefined result.
 */
const Interpretation<double>& real_interpretation();

/** @brief Evaluate over the reals. Variables are read from `env` (default: none bound). */
double evaluate_real(const Expression& expr, const Environment<double>& env = Environment<double>());

} // namespace core
} // namespace cppmath
