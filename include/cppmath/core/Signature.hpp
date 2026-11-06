#pragma once
#include "Operator.hpp"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cppmath {
namespace core {

/**
 * @brief A set of Operators: the vocabulary a term language is built from.
 *
 * The parser, printer and evaluator are all driven by a Signature, so adding
 * an operator is one `add()` call instead of a new class.
 *
 * Ownership: a Signature owns its Operators at stable addresses. Expressions
 * hold non-owning `const Operator*`, so the Signature **must outlive every
 * Expression built from it**. `standard()` returns a function-local static
 * and is always safe. Expressions from different Signatures compare operators
 * by *name*, not address.
 */
class Signature {
public:
    /// An empty signature.
    Signature() = default;
    Signature(const Signature&) = delete;            // Operator addresses are handed out; no copies
    Signature& operator=(const Signature&) = delete;
    /** @brief Moving is allowed and safe: Operators live on the heap, so their addresses survive the move. */
    Signature(Signature&&) = default;
    /** @brief Move assignment; see the move constructor. */
    Signature& operator=(Signature&&) = default;

    /**
     * @brief Register an operator and return the stored copy.
     * @throws std::invalid_argument if the name is taken, or another operator
     *         already uses the same symbol with the same notation class.
     */
    const Operator& add(Operator op);

    /** @brief Add copies of all of `other`'s operators (e.g. extend standard() with a new module's). */
    void extend(const Signature& other);

    /** @brief Lookup by canonical name; nullptr if absent. */
    const Operator* find(const std::string& name) const;
    /** @brief Lookup by canonical name; throws std::out_of_range if absent. */
    const Operator& get(const std::string& name) const;

    /// Infix operator written `symbol`, or nullptr.
    const Operator* find_infix(const std::string& symbol) const { return find_symbol(Notation::Infix, symbol); }
    /// Prefix operator written `symbol`, or nullptr.
    const Operator* find_prefix(const std::string& symbol) const { return find_symbol(Notation::Prefix, symbol); }
    /// Function-call operator named `symbol`, or nullptr.
    const Operator* find_function(const std::string& symbol) const { return find_symbol(Notation::FunctionCall, symbol); }
    /// Atom (nullary constant) written `symbol`, or nullptr.
    const Operator* find_atom(const std::string& symbol) const { return find_symbol(Notation::Atom, symbol); }

    /** @brief Symbols of all Infix/Prefix operators that the lexer must recognise as punctuation-like tokens. */
    std::vector<std::string> operator_symbols() const;

    /// Number of operators.
    std::size_t size() const { return ops_.size(); }

    /**
     * @brief The built-in real-arithmetic vocabulary.
     *
     * Infix: + - * / ^ ; prefix: - ; functions: sin cos tan exp log sqrt abs ;
     * atoms: pi e. Precedence (loosest to tightest): + - (10), * / (20),
     * prefix - (30), ^ (40, right-associative), so -x^2 means -(x^2).
     */
    static const Signature& standard();

private:
    const Operator* find_symbol(Notation n, const std::string& symbol) const;

    std::vector<std::unique_ptr<Operator>> ops_;
    std::map<std::string, const Operator*> by_name_;
    std::map<std::pair<Notation, std::string>, const Operator*> by_symbol_;
};

} // namespace core
} // namespace cppmath
