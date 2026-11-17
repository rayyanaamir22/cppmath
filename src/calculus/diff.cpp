#include "cppmath/calculus/diff.hpp"
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Bind.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/Simplify.hpp"
#include "cppmath/core/Variable.hpp"
#include <stdexcept>

namespace cppmath {
namespace calculus {

using core::ExprPtr;
namespace b = core::build;

DerivativeRules& DerivativeRules::define(const std::string& op_name, DerivativeRule rule) {
    rules_[op_name] = std::move(rule);
    return *this;
}

const DerivativeRule& DerivativeRules::get(const std::string& op_name) const {
    auto it = rules_.find(op_name);
    if (it == rules_.end())
        throw std::invalid_argument("differentiate: no derivative rule for operator '" + op_name + "'");
    return it->second;
}

namespace {

// Copies of the arguments, so a rule can build new nodes from them (the originals are const).
ExprPtr arg(const RuleContext& c, std::size_t i) { return c.node.args()[i]->clone(); }

DerivativeRules build_standard() {
    DerivativeRules r;
    r.define("add", [](const RuleContext& c) { return b::add(std::move(c.d[0]), std::move(c.d[1])); });
    r.define("sub", [](const RuleContext& c) { return b::sub(std::move(c.d[0]), std::move(c.d[1])); });
    r.define("neg", [](const RuleContext& c) { return b::neg(std::move(c.d[0])); });

    // (uv)' = u'v + uv'
    r.define("mul", [](const RuleContext& c) {
        return b::add(b::mul(std::move(c.d[0]), arg(c, 1)), b::mul(arg(c, 0), std::move(c.d[1])));
    });
    // (u/v)' = (u'v - uv') / v^2
    r.define("div", [](const RuleContext& c) {
        return b::div(b::sub(b::mul(std::move(c.d[0]), arg(c, 1)), b::mul(arg(c, 0), std::move(c.d[1]))),
                      b::pow(arg(c, 1), b::num(2)));
    });
    // Three cases for u^v, so the common ones stay simple: the exponent or the base may not depend on var.
    r.define("pow", [](const RuleContext& c) -> ExprPtr {
        bool base_depends = core::contains_variable(*c.node.args()[0], c.var);
        bool exp_depends = core::contains_variable(*c.node.args()[1], c.var);
        if (!base_depends && !exp_depends) return b::num(0);
        if (!exp_depends)   // (u^n)' = n u^(n-1) u'
            return b::mul(b::mul(arg(c, 1), b::pow(arg(c, 0), b::sub(arg(c, 1), b::num(1)))), std::move(c.d[0]));
        if (!base_depends)  // (a^v)' = a^v ln(a) v'
            return b::mul(b::mul(b::pow(arg(c, 0), arg(c, 1)), b::log(arg(c, 0))), std::move(c.d[1]));
        // (u^v)' = u^v (v' ln u + v u'/u)
        return b::mul(b::pow(arg(c, 0), arg(c, 1)),
                      b::add(b::mul(std::move(c.d[1]), b::log(arg(c, 0))),
                             b::div(b::mul(arg(c, 1), std::move(c.d[0])), arg(c, 0))));
    });

    r.define("sin", [](const RuleContext& c) { return b::mul(b::cos(arg(c, 0)), std::move(c.d[0])); });
    r.define("cos", [](const RuleContext& c) { return b::neg(b::mul(b::sin(arg(c, 0)), std::move(c.d[0]))); });
    r.define("tan", [](const RuleContext& c) { return b::div(std::move(c.d[0]), b::pow(b::cos(arg(c, 0)), b::num(2))); });
    r.define("exp", [](const RuleContext& c) { return b::mul(b::exp(arg(c, 0)), std::move(c.d[0])); });
    r.define("log", [](const RuleContext& c) { return b::div(std::move(c.d[0]), arg(c, 0)); });
    r.define("sqrt", [](const RuleContext& c) { return b::div(std::move(c.d[0]), b::mul(b::num(2), b::sqrt(arg(c, 0)))); });
    r.define("abs", [](const RuleContext& c) { return b::mul(b::div(arg(c, 0), b::abs(arg(c, 0))), std::move(c.d[0])); });

    // Constants of the signature do not depend on anything.
    r.define("pi", [](const RuleContext&) { return b::num(0); });
    r.define("e", [](const RuleContext&) { return b::num(0); });
    return r;
}

class Differentiator : public core::ExpressionVisitor {
public:
    Differentiator(const std::string& var, const DerivativeRules& rules) : var_(var), rules_(rules) {}

    ExprPtr result;

    void visit(const core::Constant&) override { result = b::num(0); }

    void visit(const core::Variable& x) override { result = b::num(x.name() == var_ ? 1 : 0); }

    void visit(const core::Apply& a) override {
        const DerivativeRule& rule = rules_.get(a.op().name());   // fail before doing any work
        std::vector<ExprPtr> d;
        d.reserve(a.args().size());
        for (const auto& child : a.args()) d.push_back(differentiate_child(*child));
        result = rule(RuleContext{a, d, var_});
    }

    void visit(const core::Bind& bind) override {
        if (bind.op().name() != "sum")
            throw std::invalid_argument("differentiate: no derivative rule for binder '" + bind.op().name() + "'");
        for (std::size_t i = 0; i + 1 < bind.operands().size(); ++i)
            if (core::contains_variable(*bind.operands()[i], var_))
                throw std::invalid_argument("differentiate: the bounds of a sum depend on '" + var_ + "'");
        if (bind.variable().name() == var_) {   // var is rebound inside: the sum does not depend on the outer one
            result = b::num(0);
            return;
        }
        // The sum is linear: differentiate under it. The bound variable keeps its name, and the
        // derivative only mentions variables the body already had, so nothing can be captured.
        std::vector<ExprPtr> operands;
        for (std::size_t i = 0; i + 1 < bind.operands().size(); ++i) operands.push_back(bind.operands()[i]->clone());
        operands.push_back(differentiate_child(bind.body()));
        result = std::make_unique<core::Bind>(bind.op(), bind.variable().name(), std::move(operands));
    }

private:
    ExprPtr differentiate_child(const core::Expression& e) {
        Differentiator sub(var_, rules_);
        e.accept(sub);
        return std::move(sub.result);
    }

    const std::string& var_;
    const DerivativeRules& rules_;
};

} // namespace

const DerivativeRules& DerivativeRules::standard() {
    static const DerivativeRules rules = build_standard();
    return rules;
}

ExprPtr differentiate(const core::Expression& expr, const std::string& var, const DerivativeRules& rules) {
    Differentiator d(var, rules);
    expr.accept(d);
    return core::simplify(*d.result);
}

} // namespace calculus
} // namespace cppmath
