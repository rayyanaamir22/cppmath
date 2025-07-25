#pragma once
#include <memory>
#include <string>

namespace cppmath {
namespace core {

class Expression {
public:
    virtual ~Expression() = default;
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<Expression> clone() const = 0;
    virtual double evaluate() const = 0; // For now, no variables
};

} // namespace core
} // namespace cppmath 