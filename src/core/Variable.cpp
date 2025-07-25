#include "cppmath/core/Variable.hpp"

namespace cppmath {
namespace core {

Variable::Variable(const std::string& n) : name(n) {}
std::string Variable::toString() const { return name; }
std::unique_ptr<Expression> Variable::clone() const { return std::make_unique<Variable>(*this); }
double Variable::evaluate() const { throw std::runtime_error("Cannot evaluate variable without value"); }

} // namespace core
} // namespace cppmath 