#pragma once
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Expression.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Calculus on symbolic expressions (differentiation so far).
 */
namespace cppmath {
namespace calculus {

/** @brief What a derivative rule is given when it differentiates one operator application. */
struct RuleContext {
    const core::Apply& node;          ///< the application being differentiated, e.g. `sin(u)`
    std::vector<core::ExprPtr>& d;    ///< d[i] is the derivative of node.args()[i]; a rule may move from it
    const std::string& var;           ///< the variable of differentiation
};

/**
 * @brief Derivative of one operator, written with the chain rule already applied.
 *
 * For `sin(u)` the rule returns `cos(u) * u'`: `node.args()[0]` is `u` and `d[0]` is `u'`.
 */
using DerivativeRule = std::function<core::ExprPtr(const RuleContext&)>;

/**
 * @brief A table of derivative rules, keyed by operator name.
 *
 * `standard()` covers every operator of `core::Signature::standard()` (`add sub mul div pow neg
 * sin cos tan exp log sqrt abs pi e`). To differentiate an operator you added to a Signature,
 * copy the standard table and `define` a rule for it:
 *
 * @code
 *   calculus::DerivativeRules rules = calculus::DerivativeRules::standard();
 *   rules.define("sq", [](const calculus::RuleContext& c) {      // d/dx u^2 = 2 u u'
 *       return core::build::mul(core::build::mul(core::build::num(2), c.node.args()[0]->clone()),
 *                               std::move(c.d[0])); });
 * @endcode
 */
class DerivativeRules {
public:
    /** @brief Add or replace the rule for the operator called `op_name`. Returns *this for chaining. */
    DerivativeRules& define(const std::string& op_name, DerivativeRule rule);
    /// Whether there is a rule for `op_name`.
    bool defines(const std::string& op_name) const { return rules_.count(op_name) > 0; }
    /** @brief The rule for `op_name`. @throws std::invalid_argument if there is none. */
    const DerivativeRule& get(const std::string& op_name) const;

    /// The rules for the standard operators. A function-local static, so always safe to use.
    static const DerivativeRules& standard();

private:
    std::map<std::string, DerivativeRule> rules_;
};

/**
 * @brief The derivative d(expr)/d(var), simplified with `core::simplify`.
 *
 * Any variable other than `var` is treated as a constant. Works through `sum` binders when the
 * bounds do not depend on `var`: d/dx sum(i, lo, hi, f) = sum(i, lo, hi, df/dx).
 *
 * Differentiate twice for a second derivative. Note the derivative of `abs` is `a/abs(a)`
 * (undefined at 0) and of `sqrt` is `1/(2*sqrt(a))`.
 *
 * @param rules rule table; defaults to `DerivativeRules::standard()`.
 * @throws std::invalid_argument if an operator has no rule, a binder other than `sum` is met,
 *         or the bounds of a `sum` depend on `var` (the sum is then not a differentiable function of it here).
 */
core::ExprPtr differentiate(const core::Expression& expr, const std::string& var,
                            const DerivativeRules& rules = DerivativeRules::standard());

} // namespace calculus
} // namespace cppmath
