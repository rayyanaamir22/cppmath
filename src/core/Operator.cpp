#include "cppmath/core/Operator.hpp"
#include <algorithm>
#include <stdexcept>

namespace cppmath {
namespace core {

Operator::Operator(std::string name, std::string symbol, Arity arity, std::vector<Sort> input_sorts,
                   Sort output_sort, Notation notation, int precedence, Assoc assoc,
                   Properties properties)
    : name_(std::move(name)), symbol_(std::move(symbol)), arity_(arity),
      input_sorts_(std::move(input_sorts)), output_sort_(std::move(output_sort)),
      notation_(notation), precedence_(precedence), assoc_(assoc), properties_(properties) {
    if (name_.empty() || symbol_.empty())
        throw std::invalid_argument("Operator: name and symbol must be non-empty");

    // Atom/Prefix/Infix have a fixed shape so the parser never needs to re-check arity.
    std::size_t required = 0;
    switch (notation_) {
        case Notation::Atom: required = 0; break;
        case Notation::Prefix: required = 1; break;
        case Notation::Infix: required = 2; break;
        case Notation::FunctionCall: required = arity_.count; break;
    }
    if (notation_ != Notation::FunctionCall && (arity_.variadic || arity_.count != required))
        throw std::invalid_argument("Operator '" + name_ + "': this notation requires fixed arity " +
                                    std::to_string(required));

    if (arity_.variadic) {
        if (input_sorts_.empty())
            throw std::invalid_argument("Operator '" + name_ + "': variadic operator needs at least one input sort");
    } else if (input_sorts_.size() != arity_.count) {
        throw std::invalid_argument("Operator '" + name_ + "': expected " + std::to_string(arity_.count) +
                                    " input sorts, got " + std::to_string(input_sorts_.size()));
    }
}

bool Operator::accepts_count(std::size_t n) const {
    return arity_.variadic ? n >= arity_.count : n == arity_.count;
}

const Sort& Operator::input_sort(std::size_t i) const {
    // Variadic operators reuse their last listed sort for all extra arguments.
    return input_sorts_[std::min(i, input_sorts_.size() - 1)];
}

} // namespace core
} // namespace cppmath
