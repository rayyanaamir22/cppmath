#pragma once
#include "Expression.hpp"
#include <string>
#include <memory>

namespace cppmath {
namespace core {

class Constant : public Expression {
public:
    double value;
    Constant(double v);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

} // namespace core
} // namespace cppmath 