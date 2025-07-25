#pragma once
#include "Expression.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace core {

class AddExpression : public Expression {
public:
    std::unique_ptr<Expression> left, right;
    AddExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

class MultiplyExpression : public Expression {
public:
    std::unique_ptr<Expression> left, right;
    MultiplyExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

class PowerExpression : public Expression {
public:
    std::unique_ptr<Expression> base, exponent;
    PowerExpression(std::unique_ptr<Expression> b, std::unique_ptr<Expression> e);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

} // namespace core
} // namespace cppmath 