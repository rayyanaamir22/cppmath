#pragma once
#include "SourceSpan.hpp"
#include <string>

namespace cppmath {
namespace core {
namespace parse {

/** @brief The kinds of token the Lexer produces. */
enum class TokenKind {
    Number,      ///< 3, 0.25, 1e-3
    Identifier,  ///< x, sin, pi
    Operator,    ///< a symbol registered in the Signature (+, -, ^, ...)
    LParen,
    RParen,
    Comma,
    End          ///< end of input (always the last token)
};

/** @brief One lexed token. */
struct Token {
    TokenKind kind;  ///< what kind of token
    std::string text;  ///< the exact source text
    double number = 0.0;  ///< value, for Number tokens
    SourceSpan span;  ///< where it came from
};

} // namespace parse
} // namespace core
} // namespace cppmath
