#pragma once
#include "Expression.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace core {

class SinExpression : public Expression {
public:
    std::unique_ptr<Expression> arg;
    SinExpression(std::unique_ptr<Expression> a);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

class CosExpression : public Expression {
public:
    std::unique_ptr<Expression> arg;
    CosExpression(std::unique_ptr<Expression> a);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

class ExpExpression : public Expression {
public:
    std::unique_ptr<Expression> arg;
    ExpExpression(std::unique_ptr<Expression> a);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

class LogExpression : public Expression {
public:
    std::unique_ptr<Expression> arg;
    LogExpression(std::unique_ptr<Expression> a);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

} // namespace core
} // namespace cppmath 