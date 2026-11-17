#pragma once
#include "Expression.hpp"

namespace cppmath {
namespace core {

/**
 * @brief A tidied copy of `expr`: constants folded and trivial identities removed.
 *
 * Rewrites bottom-up with these rules (over the standard operators `add sub mul div pow neg`
 * and a few functions); the input is never modified.
 *
 *  - **Identities:** `x+0 → x`, `x-0 → x`, `0-x → -x`, `x*1 → x`, `x*0 → 0`, `x*(-1) → -x`,
 *    `x/1 → x`, `x^1 → x`, `x^0 → 1`, `1^x → 1`, `x-x → 0`, `-(-x) → x`.
 *  - **Signs:** negations are pulled outward and cancelled: `x+(-y) → x-y`, `x-(-y) → x+y`,
 *    `(-x)*y → -(x*y)`, `(-x)*(-y) → x*y`, `x/(-y) → -(x/y)`.
 *  - **Constant folding** (only when the result is finite): `+ - * neg` of numbers; `/` when it
 *    divides exactly (`6/3 → 2`, but `1/3` is left alone); `^` with a small non-negative
 *    integer exponent; and the exact values `sin(0)`, `tan(0)`, `cos(0)`, `exp(0)`, `log(1)`,
 *    `sqrt(0)`, `sqrt(1)`, `abs(number)`. Anything inexact (`sin(1)`, `1/3`) stays symbolic.
 *  - **Binders:** their operands are simplified, the binder itself is kept.
 *
 * Negative numbers are `neg(literal)` in this library, so a folded `-3` prints as `(-3)`.
 *
 * Like most computer-algebra simplifiers it treats these identities as always valid, so it
 * can remove a sub-expression that would have failed to evaluate: `0 * log(0)` becomes `0`
 * and `x - x` becomes `0` even where `x` is undefined. The result is equal to the input
 * wherever the input is defined.
 *
 * It is idempotent: `simplify(simplify(e))` equals `simplify(e)`. It does not combine like
 * terms (`x + x` stays) or expand products.
 */
ExprPtr simplify(const Expression& expr);

} // namespace core
} // namespace cppmath
