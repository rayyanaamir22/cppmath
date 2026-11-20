#pragma once
#include "Apply.hpp"
#include "Bind.hpp"
#include "Constant.hpp"
#include "Expression.hpp"
#include "Variable.hpp"
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cppmath {
namespace core {

namespace detail {
template <class R, class OnConstant, class OnVariable, class OnApply, class OnBind>
class FoldVisitor : public ExpressionVisitor {
public:
    FoldVisitor(OnConstant& c, OnVariable& v, OnApply& a, OnBind& b) : c_(c), v_(v), a_(a), b_(b) {}

    void visit(const Constant& k) override { result.emplace(c_(k)); }
    void visit(const Variable& x) override { result.emplace(v_(x)); }
    void visit(const Apply& node) override {
        result.emplace(a_(node.op(), fold_all(node.args())));
    }
    void visit(const Bind& node) override {
        result.emplace(b_(node, fold_all(node.operands())));
    }

    std::optional<R> result;  // optional so R need not be default-constructible

private:
    // Fold every child (in order) and collect the results.
    std::vector<R> fold_all(const std::vector<ExprPtr>& children) {
        std::vector<R> folded;
        folded.reserve(children.size());
        for (const auto& child : children) {
            FoldVisitor sub(c_, v_, a_, b_);
            child->accept(sub);
            folded.push_back(std::move(*sub.result));
        }
        return folded;
    }

    OnConstant& c_;
    OnVariable& v_;
    OnApply& a_;
    OnBind& b_;
};

// The on_bind handler used by the 3-handler fold: binders are not supported there.
template <class R>
struct RejectBinders {
    R operator()(const Bind&, std::vector<R>) const {
        throw std::invalid_argument("fold: the expression contains a binder; use the fold overload that takes an on_bind handler");
    }
};
} // namespace detail

/**
 * @brief Catamorphism: collapse a tree into a value of type R, bottom-up.
 *
 * Children are folded first, then `on_apply(op, child_results)` combines
 * them; for a binder, `on_bind(bind_node, operand_results)` gets the results for
 * the bounds followed by the body (the last one). Printing, size, depth and
 * free variables are all folds. Mathematically this is the unique homomorphism
 * out of the free term algebra.
 *
 * `on_bind` receives the node itself because scoping needs the bound variable's
 * name (for example, free variables of a binder are those of its operands minus the
 * bound variable, which only applies to the body).
 *
 * @code
 *   // count nodes
 *   auto n = fold<std::size_t>(e,
 *       [](const Constant&) { return std::size_t(1); },
 *       [](const Variable&) { return std::size_t(1); },
 *       [](const Operator&, std::vector<std::size_t> kids) {
 *           return std::accumulate(kids.begin(), kids.end(), std::size_t(1)); },
 *       [](const Bind&, std::vector<std::size_t> kids) {
 *           return std::accumulate(kids.begin(), kids.end(), std::size_t(1)); });
 * @endcode
 *
 * R must be given explicitly: `fold<R>(expr, on_constant, on_variable, on_apply, on_bind)`.
 */
template <class R, class OnConstant, class OnVariable, class OnApply, class OnBind>
R fold(const Expression& expr, OnConstant on_constant, OnVariable on_variable, OnApply on_apply,
       OnBind on_bind) {
    detail::FoldVisitor<R, OnConstant, OnVariable, OnApply, OnBind> visitor(on_constant, on_variable,
                                                                           on_apply, on_bind);
    expr.accept(visitor);
    return std::move(*visitor.result);
}

/**
 * @brief Fold for trees that contain no binders: `fold<R>(expr, on_constant, on_variable, on_apply)`.
 *
 * Convenience for callers that don't deal with binders.
 * @throws std::invalid_argument if the tree does contain a Bind. Algorithms that must
 *         accept binders should use the overload with an `on_bind` handler.
 */
template <class R, class OnConstant, class OnVariable, class OnApply>
R fold(const Expression& expr, OnConstant on_constant, OnVariable on_variable, OnApply on_apply) {
    return fold<R>(expr, on_constant, on_variable, on_apply, detail::RejectBinders<R>());
}

/**
 * @brief Bottom-up rewrite: rebuild the tree, passing every rebuilt node through `rewrite`.
 *
 * `rewrite` takes and returns an ExprPtr (return the argument unchanged to
 * leave a node alone). It sees each node *after* its children were rewritten,
 * and its output is not traversed again, which makes it the building block
 * for simplification and differentiation.
 *
 * It is **not scope-aware**: inside `sum(i, 1, 3, body)` the rewrite sees the bound
 * `i` like any other variable, and it also sees Bind nodes themselves (as rebuilt
 * Bind nodes). Rewrites that care about binding, such as substitution, must handle
 * Bind explicitly (see Substitution).
 */
template <class Rewrite>
ExprPtr transform(const Expression& expr, Rewrite rewrite) {
    return fold<ExprPtr>(
        expr,
        [&](const Constant& k) { return rewrite(k.clone()); },
        [&](const Variable& x) { return rewrite(x.clone()); },
        [&](const Operator& op, std::vector<ExprPtr> kids) {
            return rewrite(std::make_unique<Apply>(op, std::move(kids)));
        },
        [&](const Bind& b, std::vector<ExprPtr> kids) {
            return rewrite(std::make_unique<Bind>(b.op(), b.variable().name(), std::move(kids)));
        });
}

} // namespace core
} // namespace cppmath
