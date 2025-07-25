#include "cppmath/core/BinaryOperations.hpp"
#include <sstream>
#include <cmath>

namespace cppmath {
namespace core {

AddExpression::AddExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : left(std::move(l)), right(std::move(r)) {}
std::string AddExpression::toString() const {
    return "(" + left->toString() + " + " + right->toString() + ")";
}
std::unique_ptr<Expression> AddExpression::clone() const {
    return std::make_unique<AddExpression>(left->clone(), right->clone());
}
double AddExpression::evaluate() const {
    return left->evaluate() + right->evaluate();
}

MultiplyExpression::MultiplyExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : left(std::move(l)), right(std::move(r)) {}
std::string MultiplyExpression::toString() const {
    return "(" + left->toString() + " * " + right->toString() + ")";
}
std::unique_ptr<Expression> MultiplyExpression::clone() const {
    return std::make_unique<MultiplyExpression>(left->clone(), right->clone());
}
double MultiplyExpression::evaluate() const {
    return left->evaluate() * right->evaluate();
}

PowerExpression::PowerExpression(std::unique_ptr<Expression> b, std::unique_ptr<Expression> e)
    : base(std::move(b)), exponent(std::move(e)) {}
std::string PowerExpression::toString() const {
    return "(" + base->toString() + " ^ " + exponent->toString() + ")";
}
std::unique_ptr<Expression> PowerExpression::clone() const {
    return std::make_unique<PowerExpression>(base->clone(), exponent->clone());
}
double PowerExpression::evaluate() const {
    return std::pow(base->evaluate(), exponent->evaluate());
}

} // namespace core
} // namespace cppmath 