#pragma once
#include "cppmath/sets/Set.hpp"
#include "Expression.hpp"
#include <string>
#include <memory>

namespace cppmath {
namespace core {

class Function {
public:
    std::string name;
    const sets::Set* domain;
    const sets::Set* codomain;
    std::unique_ptr<Expression> rule;

    Function(const std::string& n, const sets::Set* dom, const sets::Set* codom, std::unique_ptr<Expression> r);
    std::string toString() const;
    double evaluate() const;
};

} // namespace core
} // namespace cppmath 