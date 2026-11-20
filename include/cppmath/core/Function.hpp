#pragma once
#include "Expression.hpp"
#include "cppmath/sets/Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace core {

/**
 * @brief A real function f : domain -> codomain, f(param) = rule.
 *
 * `domain` and `codomain` are non-owning and may be null ("unspecified");
 * the pointed-to sets must outlive the Function. The rule is owned.
 * (Sets only test membership of a double, so Functions are single-variable.)
 */
class Function {
public:
    std::string name;  ///< the function's name, e.g. "f"
    const sets::Set* domain;  ///< non-owning; null means unspecified
    const sets::Set* codomain;  ///< non-owning; null means unspecified
    std::unique_ptr<Expression> rule;  ///< owned; never null
    std::string param;  ///< name of the variable the rule is written in

    /** @throws std::invalid_argument if `rule` is null. */
    Function(const std::string& n, const sets::Set* dom, const sets::Set* codom,
             std::unique_ptr<Expression> r, const std::string& param_name = "x");

    /** @brief e.g. `f: [0, inf) -> ℝ, f(x) = (x + 1)`; an unspecified set prints as `?`. */
    std::string toString() const;

    /**
     * @brief f(x): checks x is in the domain, evaluates the rule, checks the result is in the codomain.
     * @throws std::invalid_argument if either check fails; std::out_of_range if the rule uses
     *         a variable other than `param`.
     */
    double operator()(double x) const;
};

} // namespace core
} // namespace cppmath
