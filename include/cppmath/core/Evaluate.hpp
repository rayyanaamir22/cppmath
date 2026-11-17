#pragma once
#include "Bind.hpp"
#include "Environment.hpp"
#include "Interpretation.hpp"
#include "Apply.hpp"
#include "Constant.hpp"
#include "Variable.hpp"
#include <optional>
#include <vector>

namespace cppmath {
namespace core {

namespace detail {
/**
 * Evaluates one expression in one environment. It is a visitor, not a `fold`, because a
 * binder's body must be evaluated *lazily*, once per value of the bound variable,
 * each time in a child scope that binds that variable.
 */
template <class Carrier>
class Evaluator : public ExpressionVisitor {
public:
    Evaluator(const Interpretation<Carrier>& interp, const Environment<Carrier>& env)
        : interp_(interp), env_(env) {}

    std::optional<Carrier> result;

    void visit(const Constant& k) override { result.emplace(interp_.literal(k.value())); }

    void visit(const Variable& x) override { result.emplace(env_.lookup(x.name())); }

    void visit(const Apply& node) override {
        std::vector<Carrier> args;
        args.reserve(node.args().size());
        for (const auto& child : node.args()) args.push_back(eval(*child, env_));
        result.emplace(interp_.apply(node.op(), args));
    }

    void visit(const Bind& node) override {
        // Bounds are outside the variable's scope, so they see the enclosing environment.
        std::vector<Carrier> bounds;
        for (std::size_t i = 0; i + 1 < node.operands().size(); ++i)
            bounds.push_back(eval(*node.operands()[i], env_));

        auto body = [&](const Carrier& value) {
            Environment<Carrier> scope(&env_);          // shadows any outer variable of the same name
            scope.bind(node.variable().name(), value);
            return eval(node.body(), scope);
        };
        result.emplace(interp_.apply_binder(node.op(), bounds, body));
    }

private:
    Carrier eval(const Expression& expr, const Environment<Carrier>& env) const {
        Evaluator sub(interp_, env);
        expr.accept(sub);
        return std::move(*sub.result);
    }

    const Interpretation<Carrier>& interp_;
    const Environment<Carrier>& env_;
};
} // namespace detail

/**
 * @brief Evaluate `expr` in the carrier of `interp`, reading variables from `env`.
 *
 * This is the fold of the term into the interpretation's algebra, except that a
 * binder's body is evaluated repeatedly, in a child scope that binds the variable
 * (see Interpretation::define_binder).
 * @throws std::out_of_range for an unbound variable or an operator or binder the
 *         interpretation does not define; interpretations may throw
 *         std::invalid_argument for bad domains.
 */
template <class Carrier>
Carrier evaluate(const Expression& expr, const Interpretation<Carrier>& interp,
                 const Environment<Carrier>& env) {
    detail::Evaluator<Carrier> evaluator(interp, env);
    expr.accept(evaluator);
    return std::move(*evaluator.result);
}

} // namespace core
} // namespace cppmath
