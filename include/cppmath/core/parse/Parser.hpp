#pragma once
#include "cppmath/core/Expression.hpp"
#include "cppmath/core/Signature.hpp"
#include <string>

namespace cppmath {
namespace core {
namespace parse {

/**
 * @brief Pratt (precedence-climbing) parser: text -> Expression.
 *
 * Grammar, with all precedences and associativities read from the Signature:
 *
 *   expr    := prefix { infix-op expr }          (binds by precedence)
 *   prefix  := NUMBER | IDENT | IDENT '(' [expr {',' expr}] ')'
 *            | '(' expr ')' | prefix-op expr
 *
 * An identifier is, in order: a function call (if followed by '('), an Atom
 * of the Signature (pi, e), or a Variable. Implicit multiplication ("2x") is
 * not supported. Round-trip guarantee: `parse(e.toString())` equals `e`.
 * The Signature must outlive the Parser and the trees it returns.
 */
class Parser {
public:
    /** @param signature operators to recognise; must outlive the Parser and the trees it returns. */
    explicit Parser(const Signature& signature = Signature::standard());

    /** @throws ParseError with the position of the problem. */
    ExprPtr parse(const std::string& source) const;

private:
    const Signature& sig_;
};

} // namespace parse
} // namespace core
} // namespace cppmath
