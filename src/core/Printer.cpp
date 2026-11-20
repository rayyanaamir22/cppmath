#include "cppmath/core/Printer.hpp"
#include "cppmath/core/Fold.hpp"
#include "cppmath/utils/format.hpp"
#include <cctype>

namespace cppmath {
namespace core {

std::string format_number(double v) { return utils::format_number(v); }

std::string print(const Expression& expr) {
    return fold<std::string>(
        expr,
        [](const Constant& k) { return format_number(k.value()); },
        [](const Variable& x) { return x.name(); },
        [](const Operator& op, std::vector<std::string> kids) {
            switch (op.notation()) {
                case Notation::Atom:
                    return op.symbol();
                case Notation::Prefix: {
                    // A word operator needs a space or it would fuse with its operand: "(not x)", not "(notx)".
                    unsigned char first = static_cast<unsigned char>(op.symbol()[0]);
                    bool word = std::isalnum(first) || first == '_';
                    return "(" + op.symbol() + (word ? " " : "") + kids[0] + ")";
                }
                case Notation::Infix:
                    return "(" + kids[0] + " " + op.symbol() + " " + kids[1] + ")";
                case Notation::FunctionCall: {
                    std::string out = op.symbol() + "(";
                    for (std::size_t i = 0; i < kids.size(); ++i) out += (i ? ", " : "") + kids[i];
                    return out + ")";
                }
                case Notation::Bracket: {
                    std::string out(1, op.symbol()[0]);   // the symbol is the two delimiters, e.g. "[)"
                    for (std::size_t i = 0; i < kids.size(); ++i) out += (i ? ", " : "") + kids[i];
                    return out + op.symbol()[1];
                }
                case Notation::Binder:
                    break;  // binders are Bind nodes, printed by the handler below
            }
            return std::string();  // unreachable: all notations handled
        },
        [](const Bind& b, std::vector<std::string> kids) {
            // sum(i, 1, 10, body): the bound variable first, then the operands.
            std::string out = b.op().symbol() + "(" + b.variable().name();
            for (const auto& k : kids) out += ", " + k;
            return out + ")";
        });
}

std::string Expression::toString() const { return print(*this); }

} // namespace core
} // namespace cppmath
