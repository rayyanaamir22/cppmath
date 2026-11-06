#pragma once
#include "Apply.hpp"
#include "Constant.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include <optional>
#include <utility>
#include <vector>

namespace cppmath {
namespace core {

namespace detail {
template <class R, class OnConstant, class OnVariable, class OnApply>
class FoldVisitor : public ExpressionVisitor {
public:
    FoldVisitor(OnConstant& c, OnVariable& v, OnApply& a) : c_(c), v_(v), a_(a) {}

    void visit(const Constant& k) override { result.emplace(c_(k)); }
    void visit(const Variable& x) override { result.emplace(v_(x)); }
    void visit(const Apply& node) override {
        std::vector<R> folded;
        folded.reserve(node.args().size());
        for (const auto& child : node.args()) {
            FoldVisitor sub(c_, v_, a_);
            child->accept(sub);
            folded.push_back(std::move(*sub.result));
        }
        result.emplace(a_(node.op(), std::move(folded)));
    }

    std::optional<R> result;  // optional so R need not be default-constructible

private:
    OnConstant& c_;
    OnVariable& v_;
    OnApply& a_;
};
} // namespace detail

/**
 * @brief Catamorphism: collapse a tree into a value of type R, bottom-up.
 *
 * Children are folded first, then `on_apply(op, child_results)` combines
 * them. Evaluation, printing, size, depth and free variables are all folds.
 * Mathematically this is the unique homomorphism out of the free term algebra.
 *
 * @code
 *   // count nodes
 *   auto n = fold<std::size_t>(e,
 *       [](const Constant&) { return std::size_t(1); },
 *       [](const Variable&) { return std::size_t(1); },
 *       [](const Operator&, std::vector<std::size_t> kids) {
 *           return std::accumulate(kids.begin(), kids.end(), std::size_t(1)); });
 * @endcode
 *
 * R must be given explicitly: `fold<R>(expr, on_constant, on_variable, on_apply)`.
 */
template <class R, class OnConstant, class OnVariable, class OnApply>
R fold(const Expression& expr, OnConstant on_constant, OnVariable on_variable, OnApply on_apply) {
    detail::FoldVisitor<R, OnConstant, OnVariable, OnApply> visitor(on_constant, on_variable, on_apply);
    expr.accept(visitor);
    return std::move(*visitor.result);
}

/**
 * @brief Bottom-up rewrite: rebuild the tree, passing every rebuilt node through `rewrite`.
 *
 * `rewrite` takes and returns an ExprPtr (return the argument unchanged to
 * leave a node alone). It sees each node *after* its children were rewritten,
 * and its output is not traversed again, which makes it the building block
 * for simultaneous substitution, simplification and differentiation.
 */
template <class Rewrite>
ExprPtr transform(const Expression& expr, Rewrite rewrite) {
    return fold<ExprPtr>(
        expr,
        [&](const Constant& k) { return rewrite(k.clone()); },
        [&](const Variable& x) { return rewrite(x.clone()); },
        [&](const Operator& op, std::vector<ExprPtr> kids) {
            return rewrite(std::make_unique<Apply>(op, std::move(kids)));
        });
}

} // namespace core
} // namespace cppmath
