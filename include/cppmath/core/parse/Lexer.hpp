#pragma once
#include "Token.hpp"
#include "cppmath/core/Signature.hpp"
#include <string>
#include <vector>

namespace cppmath {
namespace core {
namespace parse {

/**
 * @brief Splits text into Tokens, using the Signature's symbols as the operator vocabulary.
 *
 * Operators are matched longest-first, so a signature with both "<" and "<="
 * lexes correctly, and new operators need no lexer changes. Symbols may be
 * UTF-8 (matched bytewise). Identifiers are ASCII: [A-Za-z_][A-Za-z0-9_]*.
 * The Signature must outlive the Lexer.
 */
class Lexer {
public:
    /** @param signature source of the operator symbols; must outlive the Lexer. */
    explicit Lexer(const Signature& signature = Signature::standard());

    /** @brief All tokens of `source`, ending with an End token. @throws ParseError on an unknown character or bad number. */
    std::vector<Token> tokenize(const std::string& source) const;

private:
    std::vector<std::string> symbols_;  // sorted longest first
};

} // namespace parse
} // namespace core
} // namespace cppmath
