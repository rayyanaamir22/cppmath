#pragma once
#include "Environment.hpp"
#include "Fold.hpp"
#include "Interpretation.hpp"

namespace cppmath {
namespace core {

/**
 * @brief Evaluate `expr` in the carrier of `interp`, reading variables from `env`.
 *
 * This is the fold of the term into the interpretation's algebra.
 * @throws std::out_of_range for an unbound variable or an operator the
 *         interpretation does not define; interpretations may throw
 *         std::invalid_argument for bad domains.
 */
template <class Carrier>
Carrier evaluate(const Expression& expr, const Interpretation<Carrier>& interp,
                 const Environment<Carrier>& env) {
    return fold<Carrier>(
        expr,
        [&](const Constant& k) { return interp.literal(k.value()); },
        [&](const Variable& x) { return env.lookup(x.name()); },
        [&](const Operator& op, std::vector<Carrier> args) { return interp.apply(op, args); });
}

} // namespace core
} // namespace cppmath
