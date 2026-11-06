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
 */
template <class Carrier>
class Interpretation {
public:
    using OperatorFn = std::function<Carrier(const std::vector<Carrier>&)>;
    using LiteralFn = std::function<Carrier(double)>;

    /** @param literal how to embed a numeric literal (e.g. 3) into the carrier. */
    explicit Interpretation(LiteralFn literal) : literal_(std::move(literal)) {}

    /** @brief Give the operator called `op_name` a meaning. Returns *this for chaining. */
    Interpretation& define(const std::string& op_name, OperatorFn fn) {
        table_[op_name] = std::move(fn);
        return *this;
    }

    /// Embed the numeric literal `v` into the carrier.
    Carrier literal(double v) const { return literal_(v); }

    /// Whether the operator called `op_name` has a meaning here.
    bool defines(const std::string& op_name) const { return table_.count(op_name) > 0; }

    /** @throws std::out_of_range if the operator has no meaning in this carrier. */
    Carrier apply(const Operator& op, const std::vector<Carrier>& args) const {
        auto it = table_.find(op.name());
        if (it == table_.end())
            throw std::out_of_range("Interpretation: operator '" + op.name() + "' is not defined");
        return it->second(args);
    }

private:
    LiteralFn literal_;
    std::map<std::string, OperatorFn> table_;
};

} // namespace core
} // namespace cppmath
