#pragma once
#include "Expression.hpp"
#include <string>

namespace cppmath {
namespace core {

/**
 * @brief Canonical text form of an expression, driven by each Operator's Notation.
 *
 *   Atom          pi
 *   Prefix        (-x)
 *   Infix         (x + y)
 *   FunctionCall  sin(x), max(x, y, z)
 *
 * Always fully parenthesised, so no precedence knowledge is needed to read it
 * back. Numbers use the shortest decimal text that parses back to the same
 * double. This is what `Expression::toString()` returns.
 */
std::string print(const Expression& expr);

/** @brief Shortest decimal text that parses back to exactly `v` (e.g. 0.1 -> "0.1"). */
std::string format_number(double v);

} // namespace core
} // namespace cppmath
