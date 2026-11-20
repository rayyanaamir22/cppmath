#include "cppmath/core/Signature.hpp"
#include <cctype>
#include <set>
#include <stdexcept>

namespace cppmath {
namespace core {

const Operator& Signature::add(Operator op) {
    if (by_name_.count(op.name()))
        throw std::invalid_argument("Signature: duplicate operator name '" + op.name() + "'");
    auto key = std::make_pair(op.notation(), op.symbol());
    if (by_symbol_.count(key))
        throw std::invalid_argument("Signature: symbol '" + op.symbol() + "' already used with this notation");

    ops_.push_back(std::make_unique<Operator>(std::move(op)));
    const Operator* stored = ops_.back().get();
    by_name_[stored->name()] = stored;
    by_symbol_[key] = stored;
    return *stored;
}

namespace {
// Words (identifier-like symbols) are lexed as identifiers; everything else as operator tokens.
bool is_word_like(const std::string& s) {
    unsigned char first = static_cast<unsigned char>(s[0]);
    return std::isalnum(first) || first == '_';
}
} // namespace

void Signature::add_alias(const std::string& alias, const std::string& op_name) {
    const Operator* op = find(op_name);
    if (!op) throw std::invalid_argument("Signature: cannot alias unknown operator '" + op_name + "'");
    if (alias.empty()) throw std::invalid_argument("Signature: an alias must be non-empty");
    if (op->notation() == Notation::Bracket)
        throw std::invalid_argument("Signature: bracket operators cannot have aliases");
    auto key = std::make_pair(op->notation(), alias);
    if (by_symbol_.count(key))
        throw std::invalid_argument("Signature: symbol '" + alias + "' already used with this notation");
    by_symbol_[key] = op;
}

void Signature::extend(const Signature& other) {
    for (const auto& op : other.ops_) add(*op);
    for (const auto& entry : other.by_symbol_)   // carry over the aliases too
        if (entry.first.second != entry.second->symbol()) add_alias(entry.first.second, entry.second->name());
}

bool Signature::is_reserved_word(const std::string& word) const {
    if (word.empty()) return false;
    if (find_atom(word) || find_function(word) || find_binder(word)) return true;
    return is_word_like(word) && (find_infix(word) || find_prefix(word));
}

const Operator* Signature::find(const std::string& name) const {
    auto it = by_name_.find(name);
    return it == by_name_.end() ? nullptr : it->second;
}

const Operator& Signature::get(const std::string& name) const {
    const Operator* op = find(name);
    if (!op) throw std::out_of_range("Signature: unknown operator '" + name + "'");
    return *op;
}

const Operator* Signature::find_symbol(Notation n, const std::string& symbol) const {
    auto it = by_symbol_.find({n, symbol});
    return it == by_symbol_.end() ? nullptr : it->second;
}

std::vector<std::string> Signature::operator_symbols() const {
    std::set<std::string> unique;   // "-" is both infix and prefix
    for (const auto& entry : by_symbol_) {   // by_symbol_ includes the aliases
        Notation n = entry.first.first;
        if (n != Notation::Infix && n != Notation::Prefix && n != Notation::Atom) continue;
        if (!is_word_like(entry.first.second)) unique.insert(entry.first.second);
    }
    return std::vector<std::string>(unique.begin(), unique.end());
}

const Signature& Signature::standard() {
    // Function-local static: built once, lives until program exit, so every
    // Expression that points into it is safe.
    static const Signature sig = [] {
        Signature s;
        const Sort R = Sort::Real();
        auto infix = [&](const char* name, const char* sym, int prec, Assoc assoc, Properties props) {
            s.add(Operator(name, sym, Arity::fixed(2), {R, R}, R, Notation::Infix, prec, assoc, props));
        };
        auto function = [&](const char* name) {
            s.add(Operator(name, name, Arity::fixed(1), {R}, R, Notation::FunctionCall));
        };
        auto atom = [&](const char* name) {
            s.add(Operator(name, name, Arity::fixed(0), {}, R, Notation::Atom));
        };

        infix("add", "+", 10, Assoc::Left, {true, true});
        infix("sub", "-", 10, Assoc::Left, {false, false});
        infix("mul", "*", 20, Assoc::Left, {true, true});
        infix("div", "/", 20, Assoc::Left, {false, false});
        infix("pow", "^", 40, Assoc::Right, {false, false});
        s.add(Operator("neg", "-", Arity::fixed(1), {R}, R, Notation::Prefix, 30));

        for (const char* f : {"sin", "cos", "tan", "exp", "log", "sqrt", "abs"}) function(f);
        atom("pi");
        atom("e");

        atom("inf");   // +infinity, so that sets::IntervalSet's text "(-inf, 0]" parses

        // ---- relations, logic and sets (sorts: Real, Bool, Set) ---------------------------------
        // Unicode is written as escapes: \u2229 ∩, \u2205 ∅, \u211d ℝ (the same text that
        // sets::Set::toString prints), and the aliases \u2208 ∈, \u222a ∪, \u2264 ≤, \u2265 ≥,
        // \u2260 ≠, \u2227 ∧, \u2228 ∨, \u00ac ¬.
        const Sort B = Sort::Bool(), St = Sort::Set(), Any = Sort::Any();
        // Precedence, loosest to tightest: or 2, and 3, not 4, relations and `in` 5, U 7, ∩ 8,
        // then the arithmetic ones above. So `x in A U B and y < 3` reads as
        // `(x in (A U B)) and (y < 3)`.
        auto relation = [&](const char* name, const char* sym) {
            s.add(Operator(name, sym, Arity::fixed(2), {R, R}, B, Notation::Infix, 5));
        };
        relation("lt", "<");
        relation("le", "<=");
        relation("gt", ">");
        relation("ge", ">=");
        // Equality works on any sort (the interpretation requires both sides to have the same one):
        // this is the one operator whose meaning depends on its operands' sorts.
        s.add(Operator("eq", "=", Arity::fixed(2), {Any, Any}, B, Notation::Infix, 5));
        s.add(Operator("ne", "!=", Arity::fixed(2), {Any, Any}, B, Notation::Infix, 5));

        s.add(Operator("or", "or", Arity::fixed(2), {B, B}, B, Notation::Infix, 2, Assoc::Left, {true, true}));
        s.add(Operator("and", "and", Arity::fixed(2), {B, B}, B, Notation::Infix, 3, Assoc::Left, {true, true}));
        s.add(Operator("not", "not", Arity::fixed(1), {B}, B, Notation::Prefix, 4));

        s.add(Operator("in", "in", Arity::fixed(2), {R, St}, B, Notation::Infix, 5));
        s.add(Operator("union", "U", Arity::fixed(2), {St, St}, St, Notation::Infix, 7, Assoc::Left, {true, true}));
        s.add(Operator("intersection", "\u2229", Arity::fixed(2), {St, St}, St, Notation::Infix, 8, Assoc::Left, {true, true}));
        s.add(Operator("complement", "complement", Arity::fixed(1), {St}, St, Notation::FunctionCall));

        s.add(Operator("empty_set", "\u2205", Arity::fixed(0), {}, St, Notation::Atom));
        s.add(Operator("universe", "\u211d", Arity::fixed(0), {}, St, Notation::Atom));
        s.add(Operator("true", "true", Arity::fixed(0), {}, B, Notation::Atom));
        s.add(Operator("false", "false", Arity::fixed(0), {}, B, Notation::Atom));

        // Intervals [a, b), (a, b], ... with Real endpoints, and finite sets {a, b, c}.
        for (const char* d : {"[]", "[)", "(]", "()"}) {
            std::string name = std::string("interval_") + (d[0] == '[' ? "c" : "o") + (d[1] == ']' ? "c" : "o");
            s.add(Operator(name, d, Arity::fixed(2), {R, R}, St, Notation::Bracket));
        }
        s.add(Operator("finite_set", "{}", Arity::at_least(0), {R}, St, Notation::Bracket));

        // Alternative spellings that are read but never printed.
        for (auto a : {std::make_pair("\u2208", "in"), std::make_pair("\u222a", "union"), std::make_pair("\u2264", "le"),
                       std::make_pair("\u2265", "ge"), std::make_pair("\u2260", "ne"), std::make_pair("==", "eq"),
                       std::make_pair("\u2227", "and"), std::make_pair("\u2228", "or"), std::make_pair("\u00ac", "not"),
                       std::make_pair("emptyset", "empty_set"), std::make_pair("reals", "universe")})
            s.add_alias(a.first, a.second);

        // sum(i, lo, hi, body) = body(lo) + ... + body(hi); prod likewise with *.
        // Operands: lo, hi, body. The bound variable i is a Real.
        for (const char* b : {"sum", "prod"})
            s.add(Operator(b, b, Arity::fixed(3), {R, R, R}, R, Notation::Binder, 0, Assoc::Left, {}, R));
        return s;
    }();
    return sig;
}

} // namespace core
} // namespace cppmath
