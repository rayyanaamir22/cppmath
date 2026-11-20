#pragma once
#include "Sort.hpp"
#include "Visitor.hpp"
#include <cstddef>
#include <memory>
#include <string>

namespace cppmath {
namespace core {

class Expression;
using ExprPtr = std::unique_ptr<Expression>;

/**
 * @brief Abstract base of the immutable syntax tree (a term over a Signature).
 *
 * Trees are owned through `std::unique_ptr` and never mutated: algorithms
 * return new trees. An Expression has *syntax only* - to give it a value, see
 * `evaluate` and `Interpretation`.
 */
class Expression {
public:
    virtual ~Expression() = default;

    /** @brief The sort of this term. */
    virtual Sort sort() const = 0;
    /** @brief Deep copy. */
    virtual ExprPtr clone() const = 0;
    /** @brief Structural equality (same shape, operators by name, same leaves). */
    virtual bool equals(const Expression& other) const = 0;
    /** @brief Hash consistent with equals(). */
    virtual std::size_t hash() const = 0;
    /** @brief Double dispatch: calls the `visitor.visit` overload for this node kind. */
    virtual void accept(ExpressionVisitor& visitor) const = 0;

    /**
     * @brief Canonical, fully parenthesised text, e.g. "(x + 1)" or "sin(x)".
     *
     * Parseable by `parse::Parser`; parsing the result gives back an equal tree.
     * Implemented once in Printer.cpp for every node kind.
     */
    std::string toString() const;
};

namespace detail {
/** @brief Boost-style hash mixing. */
inline std::size_t hash_combine(std::size_t seed, std::size_t value) {
    return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}
} // namespace detail

} // namespace core
} // namespace cppmath
