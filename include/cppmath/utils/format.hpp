#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>

namespace cppmath {
namespace utils {

/**
 * @brief Shortest decimal text that parses back to exactly `v` (0.1 -> "0.1", 5 -> "5").
 *
 * Printing with a fixed precision either loses digits (`ostream` defaults to 6
 * significant) or shows noise (`0.1` as 17 digits). This tries 15, 16, then 17
 * significant digits and keeps the first that round-trips through `strtod`,
 * which is what a parser needs. Infinities print as "inf" / "-inf".
 */
inline std::string format_number(double v) {
    char buf[40];
    for (int precision = 15; precision <= 17; ++precision) {
        std::snprintf(buf, sizeof buf, "%.*g", precision, v);
        if (std::strtod(buf, nullptr) == v) break;
    }
    return buf;
}

} // namespace utils
} // namespace cppmath
