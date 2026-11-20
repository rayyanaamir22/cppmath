#pragma once
#include <cstddef>

namespace cppmath {
namespace core {
namespace parse {

/** @brief A region of source text: byte offset/length plus 1-based line and column of its start. */
struct SourceSpan {
    std::size_t offset = 0;  ///< byte offset of the first character
    std::size_t length = 0;  ///< length in bytes
    std::size_t line = 1;  ///< 1-based line
    std::size_t column = 1;  ///< 1-based column (in bytes)
};

} // namespace parse
} // namespace core
} // namespace cppmath
