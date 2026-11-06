#include "cppmath/core/parse/Lexer.hpp"
#include "cppmath/core/parse/ParseError.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace cppmath {
namespace core {
namespace parse {

Lexer::Lexer(const Signature& signature) : symbols_(signature.operator_symbols()) {
    std::sort(symbols_.begin(), symbols_.end(),
              [](const std::string& a, const std::string& b) { return a.size() > b.size(); });
}

std::vector<Token> Lexer::tokenize(const std::string& src) const {
    std::vector<Token> tokens;
    std::size_t i = 0, line = 1, line_start = 0;  // column = i - line_start + 1

    auto span_at = [&](std::size_t start, std::size_t len) {
        return SourceSpan{start, len, line, start - line_start + 1};
    };
    auto is_digit = [&](std::size_t k) { return k < src.size() && std::isdigit(static_cast<unsigned char>(src[k])); };

    while (i < src.size()) {
        unsigned char ch = static_cast<unsigned char>(src[i]);

        if (std::isspace(ch)) {
            if (ch == '\n') { ++line; line_start = i + 1; }
            ++i;
            continue;
        }

        const std::size_t start = i;

        if (std::isdigit(ch)) {
            while (is_digit(i)) ++i;
            if (i < src.size() && src[i] == '.') {
                ++i;
                while (is_digit(i)) ++i;
            }
            // An exponent needs digits after e/E (and optional sign); otherwise the
            // 'e' is left alone so "2e" lexes as number 2 then identifier e.
            if (i < src.size() && (src[i] == 'e' || src[i] == 'E')) {
                std::size_t k = i + 1;
                if (k < src.size() && (src[k] == '+' || src[k] == '-')) ++k;
                if (is_digit(k)) {
                    i = k;
                    while (is_digit(i)) ++i;
                }
            }
            Token t{TokenKind::Number, src.substr(start, i - start), 0.0, span_at(start, i - start)};
            t.number = std::strtod(t.text.c_str(), nullptr);
            tokens.push_back(t);
            continue;
        }

        if (std::isalpha(ch) || ch == '_') {
            while (i < src.size() && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_')) ++i;
            tokens.push_back({TokenKind::Identifier, src.substr(start, i - start), 0.0, span_at(start, i - start)});
            continue;
        }

        if (ch == '(' || ch == ')' || ch == ',') {
            TokenKind kind = ch == '(' ? TokenKind::LParen : ch == ')' ? TokenKind::RParen : TokenKind::Comma;
            tokens.push_back({kind, std::string(1, src[i]), 0.0, span_at(start, 1)});
            ++i;
            continue;
        }

        bool matched = false;
        for (const std::string& sym : symbols_) {  // longest first
            if (src.compare(i, sym.size(), sym) == 0) {
                tokens.push_back({TokenKind::Operator, sym, 0.0, span_at(start, sym.size())});
                i += sym.size();
                matched = true;
                break;
            }
        }
        if (!matched) throw ParseError(std::string("unexpected character '") + src[i] + "'", span_at(start, 1));
    }

    tokens.push_back({TokenKind::End, "", 0.0, span_at(src.size(), 0)});
    return tokens;
}

} // namespace parse
} // namespace core
} // namespace cppmath
