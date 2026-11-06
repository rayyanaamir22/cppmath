#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <algorithm>
#include <stdexcept>
#include "cppmath/core/Signature.hpp"

using namespace cppmath::core;

TEST_CASE("Sort identity and wildcard") {
    CHECK(Sort::Real() == Sort("Real"));
    CHECK(Sort::Real() != Sort::Bool());
    CHECK(Sort::Real().accepts(Sort::Real()));
    CHECK_FALSE(Sort::Real().accepts(Sort::Bool()));
    CHECK(Sort::Any().accepts(Sort::Set()));
    CHECK(Sort::Real().accepts(Sort::Any()));
}

TEST_CASE("standard signature lookups") {
    const Signature& s = Signature::standard();

    CHECK(s.get("add").symbol() == "+");
    CHECK(s.find("nonsense") == nullptr);
    CHECK_THROWS_AS(s.get("nonsense"), std::out_of_range);

    // '-' is two operators, told apart by notation
    CHECK(s.find_infix("-")->name() == "sub");
    CHECK(s.find_prefix("-")->name() == "neg");
    CHECK(s.find_infix("sin") == nullptr);
    CHECK(s.find_function("sin")->name() == "sin");
    CHECK(s.find_atom("pi")->arity().count == 0);
}

TEST_CASE("standard operator metadata") {
    const Signature& s = Signature::standard();
    CHECK(s.get("mul").precedence() > s.get("add").precedence());
    CHECK(s.get("pow").precedence() > s.get("neg").precedence());
    CHECK(s.get("neg").precedence() > s.get("mul").precedence());
    CHECK(s.get("pow").assoc() == Assoc::Right);
    CHECK(s.get("add").properties().commutative);
    CHECK_FALSE(s.get("sub").properties().commutative);
    CHECK(s.get("sin").output_sort() == Sort::Real());
}

TEST_CASE("operator symbols for the lexer exclude word-like names") {
    auto syms = Signature::standard().operator_symbols();
    CHECK(std::find(syms.begin(), syms.end(), "+") != syms.end());
    CHECK(std::find(syms.begin(), syms.end(), "sin") == syms.end());
}

TEST_CASE("Operator validates its own shape") {
    const Sort R = Sort::Real();
    // infix must be binary, prefix unary, atom nullary
    CHECK_THROWS_AS(Operator("o", "o", Arity::fixed(1), {R}, R, Notation::Infix), std::invalid_argument);
    CHECK_THROWS_AS(Operator("o", "o", Arity::fixed(2), {R, R}, R, Notation::Prefix), std::invalid_argument);
    CHECK_THROWS_AS(Operator("o", "o", Arity::at_least(2), {R}, R, Notation::Infix), std::invalid_argument);
    // sort list must match arity
    CHECK_THROWS_AS(Operator("o", "o", Arity::fixed(2), {R}, R, Notation::FunctionCall), std::invalid_argument);
    CHECK_THROWS_AS(Operator("o", "o", Arity::at_least(1), {}, R, Notation::FunctionCall), std::invalid_argument);
    CHECK_THROWS_AS(Operator("", "o", Arity::fixed(0), {}, R, Notation::Atom), std::invalid_argument);
}

TEST_CASE("variadic operators reuse their last input sort") {
    const Sort R = Sort::Real();
    Operator mx("max", "max", Arity::at_least(2), {R}, R, Notation::FunctionCall);
    CHECK_FALSE(mx.accepts_count(1));
    CHECK(mx.accepts_count(2));
    CHECK(mx.accepts_count(7));
    CHECK(mx.input_sort(6) == R);
}

TEST_CASE("custom signatures: add, reject duplicates, extend") {
    const Sort R = Sort::Real();
    Signature sig;
    const Operator& f = sig.add(Operator("f", "f", Arity::fixed(1), {R}, R, Notation::FunctionCall));
    CHECK(sig.find("f") == &f);  // stable address handed back
    CHECK(sig.size() == 1);

    CHECK_THROWS_AS(sig.add(Operator("f", "g", Arity::fixed(1), {R}, R, Notation::FunctionCall)), std::invalid_argument);  // name
    CHECK_THROWS_AS(sig.add(Operator("h", "f", Arity::fixed(1), {R}, R, Notation::FunctionCall)), std::invalid_argument);  // symbol+notation
    sig.add(Operator("f_atom", "f", Arity::fixed(0), {}, R, Notation::Atom));  // same symbol, other notation: fine

    Signature ext;
    ext.extend(Signature::standard());
    ext.extend(sig);
    CHECK(ext.size() == Signature::standard().size() + sig.size());
    CHECK(ext.find("add") != Signature::standard().find("add"));  // copies, not shared
}
