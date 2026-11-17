#pragma once
#include "Expression.hpp"
#include <string>

namespace cppmath {
namespace core {

/**
 * @brief Convenience constructors for trees over Signature::standard().
 *
 * These replace the old one-class-per-operator design: `add(x, num(1))`
 * builds an Apply of the "add" operator. They live in their own namespace so
 * `using namespace cppmath::core::build;` does not clash with <cmath>.
 */
namespace build {

/** @brief A number; negatives become neg(literal) so they round-trip through the parser. */
ExprPtr num(double v);
/** @brief A Real variable. @throws std::invalid_argument for invalid names or reserved words (`pi`, `e`, `sin`, `sum`, ...). */
ExprPtr var(const std::string& name);
/// The constant pi.
ExprPtr pi();
/// The constant e (Euler's number).
ExprPtr euler();

/// (a + b)
ExprPtr add(ExprPtr a, ExprPtr b);
/// (a - b)
ExprPtr sub(ExprPtr a, ExprPtr b);
/// (a * b)
ExprPtr mul(ExprPtr a, ExprPtr b);
/// (a / b)
ExprPtr div(ExprPtr a, ExprPtr b);
/// (base ^ exponent)
ExprPtr pow(ExprPtr base, ExprPtr exponent);
/// (-a)
ExprPtr neg(ExprPtr a);

/// sin(a)
ExprPtr sin(ExprPtr a);
/// cos(a)
ExprPtr cos(ExprPtr a);
/// tan(a)
ExprPtr tan(ExprPtr a);
/// exp(a)
ExprPtr exp(ExprPtr a);
/// log(a), the natural logarithm.
ExprPtr log(ExprPtr a);
/// sqrt(a)
ExprPtr sqrt(ExprPtr a);
/// abs(a)
ExprPtr abs(ExprPtr a);

/**
 * @brief sum(variable, lo, hi, body): the sum of `body` as `variable` runs over lo..hi.
 * @throws std::invalid_argument if `variable` is invalid or reserved, or an argument is null.
 */
ExprPtr sum(const std::string& variable, ExprPtr lo, ExprPtr hi, ExprPtr body);
/** @brief prod(variable, lo, hi, body): the product of `body` as `variable` runs over lo..hi. */
ExprPtr prod(const std::string& variable, ExprPtr lo, ExprPtr hi, ExprPtr body);

} // namespace build
} // namespace core
} // namespace cppmath
