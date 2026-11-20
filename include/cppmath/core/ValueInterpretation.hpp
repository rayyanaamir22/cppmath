#pragma once
#include "Environment.hpp"
#include "Evaluate.hpp"
#include "Interpretation.hpp"
#include "Value.hpp"

namespace cppmath {
namespace core {

/**
 * @brief The whole standard vocabulary interpreted over `Value`: reals, truth values and sets.
 *
 *  - **Arithmetic, functions, `sum`/`prod`:** exactly as `real_interpretation()` (same domain errors).
 *  - **Relations** `< <= > >=` compare Reals. `=` and `!=` work on any sort but both sides must have
 *    the *same* one; comparing a Real with a Set throws `std::invalid_argument`. Sets compare
 *    structurally (see `Value::equals`).
 *  - **Logic** `and or not` act on Bools. Both operands of `and`/`or` are always evaluated (there is
 *    no short-circuiting), so `false and 1/0 < 1` still divides by zero.
 *  - **Sets:** `x in A` is `A.contains(x)`; `A U B`, `A ∩ B` and `complement(A)` use the `sets::Set`
 *    operations (which may simplify, e.g. merge overlapping intervals); `[a, b)`, `{a, b}`, `∅`, `ℝ`
 *    build `IntervalSet`, `FiniteSet`, `EmptySet` and `UniversalSet`.
 *  - **Atoms:** `true`, `false`, `inf`, `pi`, `e`.
 *
 * Applying an operation to a value of the wrong sort throws `std::invalid_argument` (the sorts
 * in an expression are checked when it is built, but an environment may bind a variable to a value
 * of another sort).
 */
const Interpretation<Value>& value_interpretation();

/**
 * @brief Evaluate `expr` over reals, bools and sets, reading variables from `env`.
 *
 * @code
 *   auto e = core::parse::Parser().parse("x in [0, 1) U {5}");
 *   core::Environment<core::Value> env;
 *   env.bind("x", core::Value::real(5));
 *   core::evaluate_value(*e, env).as_bool();   // true
 * @endcode
 * @throws std::out_of_range for an unbound variable or an operator with no meaning;
 *         std::invalid_argument for a sort mismatch or a bad domain.
 */
Value evaluate_value(const Expression& expr, const Environment<Value>& env = Environment<Value>());

} // namespace core
} // namespace cppmath
