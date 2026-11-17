#pragma once
#include "Operator.hpp"
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace cppmath {
namespace core {

/**
 * @brief A meaning for operators in some carrier type: a Sigma-algebra.
 *
 * Core never knows what "add" means. An Interpretation<double> says it is
 * `+` on doubles; an Interpretation<FiniteFieldElement> can say it is
 * addition mod p; the same parsed tree can be evaluated in either.
 *
 * Operators are matched by canonical *name*, so an interpretation works for
 * any Signature that uses the same names.
 *
 * **Binders** (`sum`, `prod`, ...) need more than a function of values, because
 * their body must be evaluated once per value of the bound variable. They are
 * given a meaning with `define_binder`, which receives the already-evaluated
 * bounds and a callback that evaluates the body for one value of the variable.
 */
template <class Carrier>
class Interpretation {
public:
    using OperatorFn = std::function<Carrier(const std::vector<Carrier>&)>;
    using LiteralFn = std::function<Carrier(double)>;
    /// Evaluates a binder's body with the bound variable set to the given value.
    using BodyFn = std::function<Carrier(const Carrier&)>;
    /// Meaning of a binder: gets the values of its bounds (everything before the body) and the body callback.
    using BinderFn = std::function<Carrier(const std::vector<Carrier>& bounds, const BodyFn& body)>;

    /** @param literal how to embed a numeric literal (e.g. 3) into the carrier. */
    explicit Interpretation(LiteralFn literal) : literal_(std::move(literal)) {}

    /** @brief Give the operator called `op_name` a meaning. Returns *this for chaining. */
    Interpretation& define(const std::string& op_name, OperatorFn fn) {
        table_[op_name] = std::move(fn);
        return *this;
    }

    /**
     * @brief Give the binder called `op_name` a meaning. Returns *this for chaining.
     *
     * @code
     *   // sum(i, lo, hi, body) over doubles
     *   interp.define_binder("sum", [](const std::vector<double>& b, const auto& body) {
     *       double acc = 0;
     *       for (double i = b[0]; i <= b[1]; i += 1) acc += body(i);
     *       return acc; });
     * @endcode
     */
    Interpretation& define_binder(const std::string& op_name, BinderFn fn) {
        binders_[op_name] = std::move(fn);
        return *this;
    }

    /// Embed the numeric literal `v` into the carrier.
    Carrier literal(double v) const { return literal_(v); }

    /// Whether the operator called `op_name` has a meaning here.
    bool defines(const std::string& op_name) const { return table_.count(op_name) > 0 || binders_.count(op_name) > 0; }

    /** @throws std::out_of_range if the operator has no meaning in this carrier. */
    Carrier apply(const Operator& op, const std::vector<Carrier>& args) const {
        auto it = table_.find(op.name());
        if (it == table_.end())
            throw std::out_of_range("Interpretation: operator '" + op.name() + "' is not defined");
        return it->second(args);
    }

    /**
     * @brief Apply the meaning of binder `op` to its evaluated bounds and a body callback.
     * @throws std::out_of_range if the binder has no meaning in this carrier.
     */
    Carrier apply_binder(const Operator& op, const std::vector<Carrier>& bounds, const BodyFn& body) const {
        auto it = binders_.find(op.name());
        if (it == binders_.end())
            throw std::out_of_range("Interpretation: operator '" + op.name() + "' is not defined");
        return it->second(bounds, body);
    }

private:
    LiteralFn literal_;
    std::map<std::string, OperatorFn> table_;
    std::map<std::string, BinderFn> binders_;
};

} // namespace core
} // namespace cppmath
