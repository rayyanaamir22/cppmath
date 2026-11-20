#pragma once
#include "Expression.hpp"
#include "Operator.hpp"
#include "Variable.hpp"
#include <vector>

namespace cppmath {
namespace core {

/**
 * @brief A binder applied to a variable and operands: `sum(i, 1, 10, (i ^ 2))`.
 *
 * Binds a variable over a body. This is how Σ, ∏ (and later ∀, ∃, ∫, λ and
 * set-builder notation) appear in a tree. The operands are the arguments after
 * the bound variable: any bounds first, then the **body** as the last operand.
 *
 * **Scope:** the variable is bound only inside the body. The bounds are
 * outside its scope, so in `sum(i, 1, i, i)` the second `i` is a *free* variable
 * of the whole expression while the third is bound. `free_variables`,
 * `Substitution` and `evaluate` all follow this rule.
 *
 * Invariant (checked at construction): the operator is a Binder, the operand
 * count and sorts agree with it, and the variable name is a valid identifier,
 * so every Bind in existence is well-sorted. The Operator is held by non-owning
 * pointer; see Signature for lifetime rules.
 *
 * `equals()` is **syntactic**: `sum(i, 1, 3, i)` and `sum(j, 1, 3, j)` are different trees
 * even though they mean the same thing. Use `alpha_equivalent` (Analysis.hpp) to compare
 * up to renaming of bound variables.
 */
class Bind : public Expression {
public:
    /**
     * @param op       a Binder operator.
     * @param variable name of the variable to bind.
     * @param operands the bounds followed by the body.
     * @throws std::invalid_argument if `op` is not a Binder, the name is not a valid
     *         identifier, an operand is null, or the operand count or sorts don't fit `op`.
     */
    Bind(const Operator& op, std::string variable, std::vector<ExprPtr> operands);

    /// The binder operator.
    const Operator& op() const { return *op_; }
    /// The bound variable (its sort comes from `op().bound_sort()`).
    const Variable& variable() const { return var_; }
    /// The bounds followed by the body (never null).
    const std::vector<ExprPtr>& operands() const { return operands_; }
    /// The last operand: the expression the variable is bound in.
    const Expression& body() const { return *operands_.back(); }

    Sort sort() const override { return op_->output_sort(); }
    ExprPtr clone() const override;
    bool equals(const Expression& other) const override;
    std::size_t hash() const override;
    void accept(ExpressionVisitor& v) const override { v.visit(*this); }

private:
    const Operator* op_;
    Variable var_;
    std::vector<ExprPtr> operands_;
};

} // namespace core
} // namespace cppmath
