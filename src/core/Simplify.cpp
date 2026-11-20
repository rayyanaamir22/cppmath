#include "cppmath/core/Simplify.hpp"
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Bind.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/Signature.hpp"
#include <cmath>
#include <vector>

namespace cppmath {
namespace core {

namespace {

// The number a literal denotes: a Constant c is c, and neg(Constant c) is -c (how negatives are written).
bool literal_value(const Expression& e, double& v) {
    if (auto* k = dynamic_cast<const Constant*>(&e)) {
        v = k->value();
        return true;
    }
    if (auto* a = dynamic_cast<const Apply*>(&e))
        if (a->op().name() == "neg")
            if (auto* k = dynamic_cast<const Constant*>(a->args()[0].get())) {
                v = -k->value();
                return true;
            }
    return false;
}

// `e` if it is an application of the operator called `name`, else nullptr.
const Apply* as_op(const Expression& e, const char* name) {
    auto* a = dynamic_cast<const Apply*>(&e);
    return a && a->op().name() == name ? a : nullptr;
}

// A literal for v. Zero is always the plain Constant 0 (never neg(0)).
ExprPtr number(double v) { return v == 0.0 ? ExprPtr(std::make_unique<Constant>(0.0)) : build::num(v); }

ExprPtr simplify_apply(const Operator& op, std::vector<ExprPtr> k);

// Build op(args) through the rules (used when a rule creates a smaller node that may simplify further).
ExprPtr make(const char* name, std::vector<ExprPtr> args) {
    return simplify_apply(Signature::standard().get(name), std::move(args));
}
ExprPtr make1(const char* name, ExprPtr a) {
    std::vector<ExprPtr> v;
    v.push_back(std::move(a));
    return make(name, std::move(v));
}
ExprPtr make2(const char* name, ExprPtr a, ExprPtr b) {
    std::vector<ExprPtr> v;
    v.push_back(std::move(a));
    v.push_back(std::move(b));
    return make(name, std::move(v));
}

// The operand of a neg node, copied.
ExprPtr inner(const Apply* neg_node) { return neg_node->args()[0]->clone(); }

ExprPtr keep(const Operator& op, std::vector<ExprPtr> k) { return std::make_unique<Apply>(op, std::move(k)); }

// Replace op(k) by the number v, unless v is not finite (then the expression stays as written).
ExprPtr fold_to(double v, const Operator& op, std::vector<ExprPtr> k) {
    return std::isfinite(v) ? number(v) : keep(op, std::move(k));
}

ExprPtr simplify_apply(const Operator& op, std::vector<ExprPtr> k) {
    const std::string& n = op.name();
    double a = 0, b = 0;
    const bool la = !k.empty() && literal_value(*k[0], a);
    const bool lb = k.size() > 1 && literal_value(*k[1], b);

    if (n == "add") {
        if (la && lb) return fold_to(a + b, op, std::move(k));
        if (la && a == 0) return std::move(k[1]);
        if (lb && b == 0) return std::move(k[0]);
        if (auto* nb = as_op(*k[1], "neg")) return make2("sub", std::move(k[0]), inner(nb));   // x + (-y) = x - y
        if (auto* na = as_op(*k[0], "neg")) return make2("sub", std::move(k[1]), inner(na));   // (-x) + y = y - x
    } else if (n == "sub") {
        if (la && lb) return fold_to(a - b, op, std::move(k));
        if (lb && b == 0) return std::move(k[0]);
        if (la && a == 0) return make1("neg", std::move(k[1]));
        if (k[0]->equals(*k[1])) return number(0);
        if (auto* nb = as_op(*k[1], "neg")) return make2("add", std::move(k[0]), inner(nb));   // x - (-y) = x + y
    } else if (n == "mul") {
        if (la && lb) return fold_to(a * b, op, std::move(k));
        if ((la && a == 0) || (lb && b == 0)) return number(0);
        if (la && a == 1) return std::move(k[1]);
        if (lb && b == 1) return std::move(k[0]);
        if (la && a == -1) return make1("neg", std::move(k[1]));
        if (lb && b == -1) return make1("neg", std::move(k[0]));
        auto* na = as_op(*k[0], "neg");
        auto* nb = as_op(*k[1], "neg");
        if (na && nb) return make2("mul", inner(na), inner(nb));                               // (-x)(-y) = xy
        if (na) return make1("neg", make2("mul", inner(na), std::move(k[1])));                 // (-x)y = -(xy)
        if (nb) return make1("neg", make2("mul", std::move(k[0]), inner(nb)));
    } else if (n == "div") {
        if (la && lb && b != 0 && std::fmod(a, b) == 0) return fold_to(a / b, op, std::move(k));   // exact quotients only
        if (lb && b == 1) return std::move(k[0]);
        if (lb && b == -1) return make1("neg", std::move(k[0]));
        auto* na = as_op(*k[0], "neg");
        auto* nb = as_op(*k[1], "neg");
        if (na && nb) return make2("div", inner(na), inner(nb));
        if (na) return make1("neg", make2("div", inner(na), std::move(k[1])));
        if (nb) return make1("neg", make2("div", std::move(k[0]), inner(nb)));
    } else if (n == "pow") {
        if (lb && b == 0) return number(1);
        if (lb && b == 1) return std::move(k[0]);
        if (la && a == 1) return number(1);
        if (la && lb && b > 0 && b <= 64 && b == std::floor(b)) return fold_to(std::pow(a, b), op, std::move(k));
        if (la && a == 0 && lb && b > 0) return number(0);
    } else if (n == "neg") {
        if (la) return number(-a);                                              // -(3) stays neg(3); -(neg 3) becomes 3
        if (auto* nn = as_op(*k[0], "neg")) return inner(nn);                   // -(-x) = x
    } else if (la) {
        // exact values of elementary functions
        if ((n == "sin" || n == "tan" || n == "sqrt") && a == 0) return number(0);
        if ((n == "cos" || n == "exp") && a == 0) return number(1);
        if (n == "log" && a == 1) return number(0);
        if (n == "sqrt" && a == 1) return number(1);
        if (n == "abs") return number(std::fabs(a));
    }
    return keep(op, std::move(k));
}

} // namespace

ExprPtr simplify(const Expression& expr) {
    if (auto* a = dynamic_cast<const Apply*>(&expr)) {
        std::vector<ExprPtr> kids;
        kids.reserve(a->args().size());
        for (const auto& child : a->args()) kids.push_back(simplify(*child));
        return simplify_apply(a->op(), std::move(kids));
    }
    if (auto* b = dynamic_cast<const Bind*>(&expr)) {
        std::vector<ExprPtr> operands;
        operands.reserve(b->operands().size());
        for (const auto& o : b->operands()) operands.push_back(simplify(*o));
        return std::make_unique<Bind>(b->op(), b->variable().name(), std::move(operands));
    }
    return expr.clone();
}

} // namespace core
} // namespace cppmath
