#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <limits>
#include <memory>
#include <stdexcept>
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/Expression.hpp"
#include "cppmath/core/Function.hpp"
#include "cppmath/core/Signature.hpp"
#include "cppmath/core/Variable.hpp"
#include "cppmath/sets/IntervalSet.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;
using namespace cppmath::sets;

TEST_CASE("Constant") {
    Constant k(5.0);
    CHECK(k.value() == 5.0);
    CHECK(k.toString() == "5");
    CHECK(k.sort() == Sort::Real());
    CHECK(Constant(0.1).toString() == "0.1");
    CHECK(Constant(0.123456789012345678).toString() == "0.12345678901234568");  // full precision survives

    CHECK_THROWS_AS(Constant(-1.0), std::invalid_argument);
    CHECK_THROWS_AS(Constant(-0.0), std::invalid_argument);
    const double nan = std::numeric_limits<double>::quiet_NaN();   // braces: a macro-wrapped
    const double inf = std::numeric_limits<double>::infinity();    // `Constant(x);` would declare a variable
    CHECK_THROWS_AS(Constant{nan}, std::invalid_argument);
    CHECK_THROWS_AS(Constant{inf}, std::invalid_argument);
}

TEST_CASE("Variable") {
    Variable v("x");
    CHECK(v.toString() == "x");
    CHECK(v.name() == "x");
    CHECK(Variable("_a1").name() == "_a1");
    CHECK_THROWS_AS(Variable("1x"), std::invalid_argument);
    CHECK_THROWS_AS(Variable(""), std::invalid_argument);
    CHECK_THROWS_AS(Variable("a b"), std::invalid_argument);
}

TEST_CASE("builders print the canonical form") {
    CHECK(add(num(5), num(3))->toString() == "(5 + 3)");
    CHECK(pow(num(2), num(3))->toString() == "(2 ^ 3)");
    CHECK(add(var("x"), num(1))->toString() == "(x + 1)");
    CHECK(sin(var("x"))->toString() == "sin(x)");
    CHECK(neg(var("x"))->toString() == "(-x)");
    CHECK(sub(var("x"), div(num(1), num(2)))->toString() == "(x - (1 / 2))");
    CHECK(mul(pi(), euler())->toString() == "(pi * e)");
    CHECK(num(-3)->toString() == "(-3)");  // negatives are neg(literal)
}

TEST_CASE("builders reject bad input") {
    CHECK_THROWS_AS(var("pi"), std::invalid_argument);   // would parse back as the constant
    CHECK_THROWS_AS(var("e"), std::invalid_argument);
    CHECK_THROWS_AS(var("not valid"), std::invalid_argument);
    const double inf = std::numeric_limits<double>::infinity();
    CHECK_THROWS_AS(num(inf), std::invalid_argument);
}

TEST_CASE("Apply checks arity, nulls and sorts at construction") {
    const Signature& s = Signature::standard();
    std::vector<ExprPtr> one;
    one.push_back(num(1));
    CHECK_THROWS_AS(Apply(s.get("add"), std::move(one)), std::invalid_argument);   // wrong count

    std::vector<ExprPtr> with_null;
    with_null.push_back(num(1));
    with_null.push_back(nullptr);
    CHECK_THROWS_AS(Apply(s.get("add"), std::move(with_null)), std::invalid_argument);

    // sort mismatch via a custom operator that wants a Bool
    Signature custom;
    custom.add(Operator("not", "not", Arity::fixed(1), {Sort::Bool()}, Sort::Bool(), Notation::FunctionCall));
    std::vector<ExprPtr> real_arg;
    real_arg.push_back(num(1));
    CHECK_THROWS_AS(Apply(custom.get("not"), std::move(real_arg)), std::invalid_argument);
}

TEST_CASE("clone is deep and equal") {
    auto e = add(var("x"), mul(num(2), num(3)));
    auto copy = e->clone();
    CHECK(copy.get() != e.get());
    CHECK(copy->equals(*e));
    CHECK(copy->hash() == e->hash());
    CHECK(copy->toString() == e->toString());
}

TEST_CASE("structural equality and hash") {
    CHECK(add(num(1), var("x"))->equals(*add(num(1), var("x"))));
    CHECK_FALSE(add(num(1), var("x"))->equals(*add(var("x"), num(1))));  // syntactic, not up to commutativity
    CHECK_FALSE(add(num(1), num(2))->equals(*mul(num(1), num(2))));
    CHECK_FALSE(num(1)->equals(*var("x")));
    CHECK_FALSE(var("x")->equals(*var("y")));
    CHECK(sin(var("x"))->hash() == sin(var("x"))->hash());
    CHECK(sin(var("x"))->hash() != cos(var("x"))->hash());
}

TEST_CASE("equality compares operators by name across signatures") {
    Signature copy;
    copy.extend(Signature::standard());
    std::vector<ExprPtr> args;
    args.push_back(num(1));
    args.push_back(num(2));
    Apply from_copy(copy.get("add"), std::move(args));
    CHECK(from_copy.equals(*add(num(1), num(2))));
}

TEST_CASE("Function: domain, codomain, rule") {
    auto domain = IntervalSet::non_negative_real();
    auto codomain = IntervalSet::closed(0, 10);
    Function f("f", domain.get(), codomain.get(), add(var("x"), num(1)));

    CHECK(f.name == "f");
    CHECK(f.toString() == "f: [0, inf) -> [0, 10], f(x) = (x + 1)");
    CHECK(f(2.0) == doctest::Approx(3.0));
    CHECK_THROWS_AS(f(-1.0), std::invalid_argument);   // outside domain
    CHECK_THROWS_AS(f(20.0), std::invalid_argument);   // result 21 outside codomain
}

TEST_CASE("Function: custom parameter name, unspecified sets, bad rule") {
    Function g("g", nullptr, nullptr, mul(var("t"), var("t")), "t");
    CHECK(g(3.0) == doctest::Approx(9.0));
    CHECK(g.toString() == "g: ? -> ?, g(t) = (t * t)");

    Function h("h", nullptr, nullptr, var("y"));  // y is not the parameter
    CHECK_THROWS_AS(h(1.0), std::out_of_range);

    CHECK_THROWS_AS(Function("k", nullptr, nullptr, nullptr), std::invalid_argument);
}
