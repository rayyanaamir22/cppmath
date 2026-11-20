#pragma once
#include "Expression.hpp"

namespace cppmath {
namespace core {

/**
 * @brief A numeric literal such as 5 or 0.25 (sort Real).
 *
 * Invariant: the value is finite and not negative (and not -0.0). A literal
 * is a *numeral*; the text "-3" is parsed as neg(3), so allowing negative
 * Constants would make print/parse round-trips lose equality. Use
 * `build::num(-3)` to get the right tree for a negative number.
 */
class Constant : public Expression {
public:
    /** @throws std::invalid_argument if v is NaN, infinite, or has its sign bit set. */
    explicit Constant(double v);

    /// The literal's value (finite, non-negative).
    double value() const { return value_; }

    Sort sort() const override { return Sort::Real(); }
    ExprPtr clone() const override;
    bool equals(const Expression& other) const override;
    std::size_t hash() const override;
    void accept(ExpressionVisitor& v) const override { v.visit(*this); }

private:
    double value_;
};

} // namespace core
} // namespace cppmath
