#include "cppmath/core/UnaryOperations.hpp"
#include <sstream>
#include <cmath>

namespace cppmath {
namespace core {

SinExpression::SinExpression(std::unique_ptr<Expression> a) : arg(std::move(a)) {}
std::string SinExpression::toString() const {
    return "sin(" + arg->toString() + ")";
}
std::unique_ptr<Expression> SinExpression::clone() const {
    return std::make_unique<SinExpression>(arg->clone());
}
double SinExpression::evaluate() const {
    return std::sin(arg->evaluate());
}

CosExpression::CosExpression(std::unique_ptr<Expression> a) : arg(std::move(a)) {}
std::string CosExpression::toString() const {
    return "cos(" + arg->toString() + ")";
}
std::unique_ptr<Expression> CosExpression::clone() const {
    return std::make_unique<CosExpression>(arg->clone());
}
double CosExpression::evaluate() const {
    return std::cos(arg->evaluate());
}

ExpExpression::ExpExpression(std::unique_ptr<Expression> a) : arg(std::move(a)) {}
std::string ExpExpression::toString() const {
    return "exp(" + arg->toString() + ")";
}
std::unique_ptr<Expression> ExpExpression::clone() const {
    return std::make_unique<ExpExpression>(arg->clone());
}
double ExpExpression::evaluate() const {
    return std::exp(arg->evaluate());
}

LogExpression::LogExpression(std::unique_ptr<Expression> a) : arg(std::move(a)) {}
std::string LogExpression::toString() const {
    return "log(" + arg->toString() + ")";
}
std::unique_ptr<Expression> LogExpression::clone() const {
    return std::make_unique<LogExpression>(arg->clone());
}
double LogExpression::evaluate() const {
    return std::log(arg->evaluate());
}

} // namespace core
} // namespace cppmath 