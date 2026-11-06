#include "cppmath/core/Signature.hpp"
#include <cctype>
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

void Signature::extend(const Signature& other) {
    for (const auto& op : other.ops_) add(*op);
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
    std::vector<std::string> out;
    for (const auto& op : ops_) {
        if (op->notation() != Notation::Infix && op->notation() != Notation::Prefix) continue;
        unsigned char first = static_cast<unsigned char>(op->symbol()[0]);
        // Word-like symbols are read as identifiers, not operator punctuation.
        if (std::isalnum(first) || first == '_') continue;
        out.push_back(op->symbol());
    }
    return out;
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
        return s;
    }();
    return sig;
}

} // namespace core
} // namespace cppmath
