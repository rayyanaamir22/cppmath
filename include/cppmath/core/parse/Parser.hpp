#pragma once
#include "cppmath/core/Expression.hpp"
#include "cppmath/core/Signature.hpp"
#include <map>
#include <string>

namespace cppmath {
namespace core {
namespace parse {

/** @brief The sort of each variable name that is not a Real: `{{"A", Sort::Set()}}` declares a set variable A. */
using SortDeclarations = std::map<std::string, Sort>;

/**
 * @brief Pratt (precedence-climbing) parser: text -> Expression.
 *
 * Grammar, with all precedences and associativities read from the Signature:
 *
 *   expr    := prefix { infix-op expr }          (binds by precedence)
 *   prefix  := NUMBER | IDENT | IDENT '(' [expr {',' expr}] ')'
 *            | '(' expr ')' | prefix-op expr
 *            | BINDER '(' IDENT ',' expr {',' expr} ')'      e.g. sum(i, 1, 10, i ^ 2)
 *            | '[' expr ',' expr (')' | ']')                 intervals  [a, b)  [a, b]
 *            | '(' expr ',' expr (')' | ']')                 intervals  (a, b)  (a, b]
 *            | '{' [expr {',' expr}] '}'                     finite set {a, b, c}
 *
 * An identifier is, in order: a function call or binder (if followed by '('), an Atom
 * of the Signature (pi, e, true, ∅, ...), or a Variable. Operators may be symbols (`+`, `∩`) or
 * words (`U`, `and`, `in`, `not`). `(` starts either a grouping or an interval; a comma after the
 * first expression decides it. In a binder the first argument is the
 * name of the variable it binds, followed by its operands (bounds, then the body). Implicit multiplication ("2x") is
 * not supported. Round-trip guarantee: `parse(e.toString())` equals `e`.
 * The Signature must outlive the Parser and the trees it returns.
 */
class Parser {
public:
    /** @param signature operators to recognise; must outlive the Parser and the trees it returns. */
    explicit Parser(const Signature& signature = Signature::standard());

    /**
     * @brief Parse `source`; every variable is a Real.
     * @throws ParseError with the position of the problem.
     */
    ExprPtr parse(const std::string& source) const;

    /**
     * @brief Parse `source`, giving the variables named in `declared` the sorts listed there.
     *
     * Needed for anything but Reals, because the sort of a variable can't be read from the text:
     * `A U B` is only well-sorted if A and B are declared as sets.
     * @throws ParseError with the position of the problem (including a sort mismatch).
     */
    ExprPtr parse(const std::string& source, const SortDeclarations& declared) const;

private:
    const Signature& sig_;
};

} // namespace parse
} // namespace core
} // namespace cppmath
