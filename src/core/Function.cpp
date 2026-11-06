#include "cppmath/core/Function.hpp"
#include "cppmath/core/Printer.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include <stdexcept>

namespace cppmath {
namespace core {

Function::Function(const std::string& n, const sets::Set* dom, const sets::Set* codom,
                   std::unique_ptr<Expression> r, const std::string& param_name)
    : name(n), domain(dom), codomain(codom), rule(std::move(r)), param(param_name) {
    if (!rule) throw std::invalid_argument("Function '" + name + "': rule must not be null");
}

std::string Function::toString() const {
    return name + ": " + (domain ? domain->toString() : "?") + " -> " +
           (codomain ? codomain->toString() : "?") + ", " + name + "(" + param + ") = " + rule->toString();
}

double Function::operator()(double x) const {
    if (domain && !domain->contains(x))
        throw std::invalid_argument(name + ": " + format_number(x) + " is outside the domain " + domain->toString());
    Environment<double> env;
    env.bind(param, x);
    double y = evaluate_real(*rule, env);
    if (codomain && !codomain->contains(y))
        throw std::invalid_argument(name + ": value " + format_number(y) + " is outside the codomain " + codomain->toString());
    return y;
}

} // namespace core
} // namespace cppmath
