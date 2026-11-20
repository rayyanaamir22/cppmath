#pragma once
#include "Expression.hpp"
#include <string>
#include <vector>

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
/**
 * @brief A variable of the given sort, e.g. a Set variable `A`. Parse with the same declaration
 *        (`parse::SortDeclarations`) so the text reads back as the same tree.
 * @throws std::invalid_argument for invalid names or reserved words.
 */
ExprPtr var(const std::string& name, Sort sort);
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

/// The constants `true` and `false` (sort Bool).
ExprPtr boolean(bool value);
/// +infinity (the atom `inf`); `neg(infinity())` is -infinity.
ExprPtr infinity();

/// The empty set ∅ (sort Set).
ExprPtr empty_set();
/// The whole real line ℝ (sort Set).
ExprPtr reals();
/**
 * @brief The interval with endpoints `lo`, `hi`: `[lo, hi]`, `[lo, hi)`, `(lo, hi]` or `(lo, hi)`.
 * @param lo   lower endpoint.
 * @param hi   upper endpoint.
 * @param lo_closed whether `lo` belongs to the interval.
 * @param hi_closed whether `hi` belongs to the interval.
 */
ExprPtr interval(ExprPtr lo, ExprPtr hi, bool lo_closed, bool hi_closed);
/// The finite set `{e1, e2, ...}` of Real expressions (possibly empty).
ExprPtr finite_set(std::vector<ExprPtr> elements);
/// a U b
ExprPtr set_union(ExprPtr a, ExprPtr b);
/// a ∩ b
ExprPtr intersection(ExprPtr a, ExprPtr b);
/// complement(a)
ExprPtr complement(ExprPtr a);
/// `x in set` (membership; sort Bool).
ExprPtr in(ExprPtr x, ExprPtr set);

/// a = b. Accepts any sorts; evaluation requires both sides to have the same one.
ExprPtr eq(ExprPtr a, ExprPtr b);
/// a != b (same sort rule as `eq`).
ExprPtr ne(ExprPtr a, ExprPtr b);
/// a < b
ExprPtr lt(ExprPtr a, ExprPtr b);
/// a <= b
ExprPtr le(ExprPtr a, ExprPtr b);
/// a > b
ExprPtr gt(ExprPtr a, ExprPtr b);
/// a >= b
ExprPtr ge(ExprPtr a, ExprPtr b);

/// a and b (Bool operands; both sides are always evaluated).
ExprPtr logical_and(ExprPtr a, ExprPtr b);
/// a or b
ExprPtr logical_or(ExprPtr a, ExprPtr b);
/// not a
ExprPtr logical_not(ExprPtr a);

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
