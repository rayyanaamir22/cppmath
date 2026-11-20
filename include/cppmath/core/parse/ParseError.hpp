#pragma once
#include "SourceSpan.hpp"
#include <stdexcept>
#include <string>

namespace cppmath {
namespace core {
namespace parse {

/** @brief Malformed input text. what() is "line:column: message"; span() locates the problem. */
class ParseError : public std::invalid_argument {
public:
    /** @param message what went wrong; @param span where. */
    ParseError(const std::string& message, SourceSpan span)
        : std::invalid_argument(std::to_string(span.line) + ":" + std::to_string(span.column) + ": " + message),
          span_(span) {}

    /// Location of the problem in the source text.
    const SourceSpan& span() const { return span_; }

private:
    SourceSpan span_;
};

} // namespace parse
} // namespace core
} // namespace cppmath
