#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Apply.hpp"
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

ExprPtr var(const std::string& name) {
    if (Signature::standard().find_atom(name))
        throw std::invalid_argument("var: '" + name + "' is a reserved constant");
    return std::make_unique<Variable>(name);
}

ExprPtr pi() { return std::make_unique<Apply>(Signature::standard().get("pi"), std::vector<ExprPtr>()); }
ExprPtr euler() { return std::make_unique<Apply>(Signature::standard().get("e"), std::vector<ExprPtr>()); }

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

} // namespace build
} // namespace core
} // namespace cppmath
