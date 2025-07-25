#include "cppmath/core/Function.hpp"

namespace cppmath {
namespace core {

Function::Function(const std::string& n, const sets::Set* dom, const sets::Set* codom, std::unique_ptr<Expression> r)
    : name(n), domain(dom), codomain(codom), rule(std::move(r)) {}

std::string Function::toString() const {
    return name + ": " + (domain ? domain->toString() : "?") + " -> " + (codomain ? codomain->toString() : "?") + ", " + name + "(x) = " + (rule ? rule->toString() : "?");
}
double Function::evaluate() const {
    return rule ? rule->evaluate() : 0.0;
}

} // namespace core
} // namespace cppmath 