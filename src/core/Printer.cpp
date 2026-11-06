#include "cppmath/core/Printer.hpp"
#include "cppmath/core/Fold.hpp"
#include <cstdio>
#include <cstdlib>

namespace cppmath {
namespace core {

std::string format_number(double v) {
    // Try increasing precision until the text round-trips; 17 digits always does.
    char buf[40];
    for (int precision = 15; precision <= 17; ++precision) {
        std::snprintf(buf, sizeof buf, "%.*g", precision, v);
        if (std::strtod(buf, nullptr) == v) break;
    }
    return buf;
}

std::string print(const Expression& expr) {
    return fold<std::string>(
        expr,
        [](const Constant& k) { return format_number(k.value()); },
        [](const Variable& x) { return x.name(); },
        [](const Operator& op, std::vector<std::string> kids) {
            switch (op.notation()) {
                case Notation::Atom:
                    return op.symbol();
                case Notation::Prefix:
                    return "(" + op.symbol() + kids[0] + ")";
                case Notation::Infix:
                    return "(" + kids[0] + " " + op.symbol() + " " + kids[1] + ")";
                case Notation::FunctionCall: {
                    std::string out = op.symbol() + "(";
                    for (std::size_t i = 0; i < kids.size(); ++i) out += (i ? ", " : "") + kids[i];
                    return out + ")";
                }
            }
            return std::string();  // unreachable: all notations handled
        });
}

std::string Expression::toString() const { return print(*this); }

} // namespace core
} // namespace cppmath
