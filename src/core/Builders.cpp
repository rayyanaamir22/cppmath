#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Bind.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/Signature.hpp"
#include "cppmath/core/Variable.hpp"
#include <cmath>
#include <stdexcept>

namespace cppmath {
namespace core {
namespace build {

namespace {
ExprPtr make(const char* op_name, ExprPtr a) {
    std::vector<ExprPtr> args;
    args.push_back(std::move(a));
    return std::make_unique<Apply>(Signature::standard().get(op_name), std::move(args));
}
ExprPtr make(const char* op_name, ExprPtr a, ExprPtr b) {
    std::vector<ExprPtr> args;
    args.push_back(std::move(a));
    args.push_back(std::move(b));
    return std::make_unique<Apply>(Signature::standard().get(op_name), std::move(args));
}
} // namespace

ExprPtr num(double v) {
    ExprPtr literal = std::make_unique<Constant>(std::fabs(v));  // throws for NaN / infinity
    return std::signbit(v) ? neg(std::move(literal)) : std::move(literal);
}

namespace {
ExprPtr make_binder(const char* op_name, const std::string& variable, ExprPtr lo, ExprPtr hi, ExprPtr body) {
    if (Signature::standard().is_reserved_word(variable))
        throw std::invalid_argument("'" + variable + "' is a reserved word and cannot be bound");
    std::vector<ExprPtr> operands;
    operands.push_back(std::move(lo));
    operands.push_back(std::move(hi));
    operands.push_back(std::move(body));
    return std::make_unique<Bind>(Signature::standard().get(op_name), variable, std::move(operands));
}
} // namespace

ExprPtr var(const std::string& name) {
    // A reserved word would print as itself but parse as the constant/function/binder.
    if (Signature::standard().is_reserved_word(name))
        throw std::invalid_argument("var: '" + name + "' is a reserved word");
    return std::make_unique<Variable>(name);
}

ExprPtr var(const std::string& name, Sort sort) {
    if (Signature::standard().is_reserved_word(name))
        throw std::invalid_argument("var: '" + name + "' is a reserved word");
    return std::make_unique<Variable>(name, std::move(sort));
}

ExprPtr pi() { return std::make_unique<Apply>(Signature::standard().get("pi"), std::vector<ExprPtr>()); }
ExprPtr euler() { return std::make_unique<Apply>(Signature::standard().get("e"), std::vector<ExprPtr>()); }

namespace {
ExprPtr atom(const char* op_name) {
    return std::make_unique<Apply>(Signature::standard().get(op_name), std::vector<ExprPtr>());
}
} // namespace

ExprPtr boolean(bool value) { return atom(value ? "true" : "false"); }
ExprPtr infinity() { return atom("inf"); }
ExprPtr empty_set() { return atom("empty_set"); }
ExprPtr reals() { return atom("universe"); }

ExprPtr interval(ExprPtr lo, ExprPtr hi, bool lo_closed, bool hi_closed) {
    const char* name = lo_closed ? (hi_closed ? "interval_cc" : "interval_co") : (hi_closed ? "interval_oc" : "interval_oo");
    return make(name, std::move(lo), std::move(hi));
}
ExprPtr finite_set(std::vector<ExprPtr> elements) {
    return std::make_unique<Apply>(Signature::standard().get("finite_set"), std::move(elements));
}
ExprPtr set_union(ExprPtr a, ExprPtr b) { return make("union", std::move(a), std::move(b)); }
ExprPtr intersection(ExprPtr a, ExprPtr b) { return make("intersection", std::move(a), std::move(b)); }
ExprPtr complement(ExprPtr a) { return make("complement", std::move(a)); }
ExprPtr in(ExprPtr x, ExprPtr set) { return make("in", std::move(x), std::move(set)); }

ExprPtr eq(ExprPtr a, ExprPtr b) { return make("eq", std::move(a), std::move(b)); }
ExprPtr ne(ExprPtr a, ExprPtr b) { return make("ne", std::move(a), std::move(b)); }
ExprPtr lt(ExprPtr a, ExprPtr b) { return make("lt", std::move(a), std::move(b)); }
ExprPtr le(ExprPtr a, ExprPtr b) { return make("le", std::move(a), std::move(b)); }
ExprPtr gt(ExprPtr a, ExprPtr b) { return make("gt", std::move(a), std::move(b)); }
ExprPtr ge(ExprPtr a, ExprPtr b) { return make("ge", std::move(a), std::move(b)); }

ExprPtr logical_and(ExprPtr a, ExprPtr b) { return make("and", std::move(a), std::move(b)); }
ExprPtr logical_or(ExprPtr a, ExprPtr b) { return make("or", std::move(a), std::move(b)); }
ExprPtr logical_not(ExprPtr a) { return make("not", std::move(a)); }

ExprPtr add(ExprPtr a, ExprPtr b) { return make("add", std::move(a), std::move(b)); }
ExprPtr sub(ExprPtr a, ExprPtr b) { return make("sub", std::move(a), std::move(b)); }
ExprPtr mul(ExprPtr a, ExprPtr b) { return make("mul", std::move(a), std::move(b)); }
ExprPtr div(ExprPtr a, ExprPtr b) { return make("div", std::move(a), std::move(b)); }
ExprPtr pow(ExprPtr b, ExprPtr e) { return make("pow", std::move(b), std::move(e)); }
ExprPtr neg(ExprPtr a) { return make("neg", std::move(a)); }

ExprPtr sin(ExprPtr a) { return make("sin", std::move(a)); }
ExprPtr cos(ExprPtr a) { return make("cos", std::move(a)); }
ExprPtr tan(ExprPtr a) { return make("tan", std::move(a)); }
ExprPtr exp(ExprPtr a) { return make("exp", std::move(a)); }
ExprPtr log(ExprPtr a) { return make("log", std::move(a)); }
ExprPtr sqrt(ExprPtr a) { return make("sqrt", std::move(a)); }
ExprPtr abs(ExprPtr a) { return make("abs", std::move(a)); }

ExprPtr sum(const std::string& variable, ExprPtr lo, ExprPtr hi, ExprPtr body) {
    return make_binder("sum", variable, std::move(lo), std::move(hi), std::move(body));
}
ExprPtr prod(const std::string& variable, ExprPtr lo, ExprPtr hi, ExprPtr body) {
    return make_binder("prod", variable, std::move(lo), std::move(hi), std::move(body));
}

} // namespace build
} // namespace core
} // namespace cppmath
