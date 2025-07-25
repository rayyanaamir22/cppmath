#pragma once
#include "Expression.hpp"
#include <string>
#include <memory>
#include <stdexcept>

namespace cppmath {
namespace core {

class Variable : public Expression {
public:
    std::string name;
    Variable(const std::string& n);
    std::string toString() const override;
    std::unique_ptr<Expression> clone() const override;
    double evaluate() const override;
};

} // namespace core
} // namespace cppmath 