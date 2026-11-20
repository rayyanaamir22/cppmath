#pragma once
#include "Expression.hpp"
#include "Operator.hpp"
#include <vector>

namespace cppmath {
namespace core {

/**
 * @brief An operator applied to arguments: the one node kind for every operation.
 *
 * `Apply(add, {x, 1})` is "(x + 1)"; `Apply(sin, {x})` is "sin(x)"; an Atom
 * like pi is an Apply with no arguments.
 *
 * Invariant: the argument count and the sorts of the arguments agree with the
 * operator (checked at construction), so every Apply in existence is well-sorted.
 * The Operator is held by non-owning pointer; see Signature for lifetime rules.
 */
class Apply : public Expression {
public:
    /** @throws std::invalid_argument on a null argument, wrong argument count, or sort mismatch. */
    Apply(const Operator& op, std::vector<ExprPtr> args);

    /// The operator being applied.
    const Operator& op() const { return *op_; }
    /// The arguments, in order (never null).
    const std::vector<ExprPtr>& args() const { return args_; }

    Sort sort() const override { return op_->output_sort(); }
    ExprPtr clone() const override;
    bool equals(const Expression& other) const override;
    std::size_t hash() const override;
    void accept(ExpressionVisitor& v) const override { v.visit(*this); }

private:
    const Operator* op_;
    std::vector<ExprPtr> args_;
};

} // namespace core
} // namespace cppmath
