#pragma once
#include "Sort.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace cppmath {
namespace core {

/** @brief How an operator is written (and therefore how it is printed and parsed). */
enum class Notation {
    Atom,         ///< nullary constant:   pi
    Prefix,       ///< one operand:        -x
    Infix,        ///< two operands:       x + y
    FunctionCall  ///< any number:         sin(x), max(x, y, z)
};

/** @brief Grouping of a chain of equal-precedence infix operators. */
enum class Assoc {
    Left,   ///< a - b - c  groups as  (a - b) - c
    Right   ///< a ^ b ^ c  groups as  a ^ (b ^ c)
};

/** @brief How many arguments an operator takes: exactly `count`, or at least `count` if variadic. */
struct Arity {
    std::size_t count;  ///< exact count, or the minimum if variadic
    bool variadic;  ///< true if more than `count` arguments are allowed
    /// Exactly `n` arguments.
    static Arity fixed(std::size_t n) { return {n, false}; }
    /// `n` or more arguments (variadic).
    static Arity at_least(std::size_t n) { return {n, true}; }
};

/**
 * @brief Syntactic facts about an operator, usable by simplifiers.
 *
 * These mirror `abstract_algebra::AlgebraicOperation::is_associative` and
 * `is_commutative` but are plain flags: core does not depend on that module.
 */
struct Properties {
    bool associative = false;  ///< (a op b) op c == a op (b op c)
    bool commutative = false;  ///< a op b == b op a
};

/**
 * @brief Immutable description of an operation symbol  f : s1 x ... x sn -> s.
 *
 * An Operator carries *no* evaluation code. What it means is decided by an
 * Interpretation; what its derivative is is decided by the calculus module.
 * Core only knows its shape: name, arity, sorts and notation.
 *
 * Invariants (checked in the constructor, throws std::invalid_argument):
 *  - Atom/Prefix/Infix operators have fixed arity 0/1/2, so the parser can
 *    build them without extra checks.
 *  - A non-variadic operator lists exactly `arity.count` input sorts. A variadic
 *    one lists at least one; the last is reused for every extra argument.
 */
class Operator {
public:
    /**
     * @param name        canonical name, unique within a Signature.
     * @param symbol      printed token (the function name for FunctionCall).
     * @param arity       argument count.
     * @param input_sorts sort of each argument (see the class invariants).
     * @param output_sort sort of the result.
     * @param notation    how it is written.
     * @param precedence  binding strength for Infix/Prefix; higher binds tighter.
     * @param assoc       grouping of chained Infix uses.
     * @param properties  algebraic hints for simplifiers.
     * @throws std::invalid_argument if the invariants above are violated.
     */
    Operator(std::string name, std::string symbol, Arity arity, std::vector<Sort> input_sorts,
             Sort output_sort, Notation notation, int precedence = 0, Assoc assoc = Assoc::Left,
             Properties properties = {});

    /** @brief Canonical identifier, unique within a Signature (e.g. "add"). */
    const std::string& name() const { return name_; }
    /** @brief The printed/lexed token (e.g. "+"). For FunctionCall operators this is the function name. */
    const std::string& symbol() const { return symbol_; }
    /// Allowed argument count.
    Arity arity() const { return arity_; }
    /// Sort of the result.
    const Sort& output_sort() const { return output_sort_; }
    /// How the operator is written.
    Notation notation() const { return notation_; }
    /** @brief Binding strength for Infix/Prefix operators; higher binds tighter. */
    int precedence() const { return precedence_; }
    /// Grouping of chained Infix uses.
    Assoc assoc() const { return assoc_; }
    /// Associativity/commutativity hints.
    const Properties& properties() const { return properties_; }

    /** @brief Whether `n` arguments are allowed. */
    bool accepts_count(std::size_t n) const;
    /** @brief The sort the i-th argument must have. Requires accepts_count(i + 1). */
    const Sort& input_sort(std::size_t i) const;

private:
    std::string name_, symbol_;
    Arity arity_;
    std::vector<Sort> input_sorts_;
    Sort output_sort_;
    Notation notation_;
    int precedence_;
    Assoc assoc_;
    Properties properties_;
};

} // namespace core
} // namespace cppmath
