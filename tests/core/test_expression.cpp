#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <memory>
#include <stdexcept>
#include "cppmath/core/Expression.hpp"
#include "cppmath/core/Variable.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/BinaryOperations.hpp"
#include "cppmath/core/UnaryOperations.hpp"
#include "cppmath/core/Function.hpp"
#include "cppmath/sets/IntervalSet.hpp"

using namespace cppmath::core;
using namespace cppmath::sets;

namespace {
std::unique_ptr<Expression> c(double v) { return std::make_unique<Constant>(v); }
std::unique_ptr<Expression> x() { return std::make_unique<Variable>("x"); }
}

TEST_CASE("Constant and Variable") {
    Constant k(5.0);
    CHECK(k.evaluate() == doctest::Approx(5.0));

    Variable v("x");
    CHECK(v.toString() == "x");
    CHECK_THROWS_AS(v.evaluate(), std::runtime_error);
}

TEST_CASE("Binary operations") {
    AddExpression add(c(5.0), c(3.0));
    CHECK(add.evaluate() == doctest::Approx(8.0));
    CHECK(add.toString() == "(5 + 3)");

    MultiplyExpression mul(c(4.0), c(2.0));
    CHECK(mul.evaluate() == doctest::Approx(8.0));

    PowerExpression pw(c(2.0), c(3.0));
    CHECK(pw.evaluate() == doctest::Approx(8.0));
    CHECK(pw.toString() == "(2 ^ 3)");
}

TEST_CASE("Unary operations") {
    CHECK(SinExpression(c(0.0)).evaluate() == doctest::Approx(0.0));
    CHECK(CosExpression(c(0.0)).evaluate() == doctest::Approx(1.0));
    CHECK(ExpExpression(c(1.0)).evaluate() == doctest::Approx(2.718281828));
    CHECK(LogExpression(c(2.718281828)).evaluate() == doctest::Approx(1.0));
    CHECK(SinExpression(x()).toString() == "sin(x)");
}

TEST_CASE("clone produces an independent equal tree") {
    AddExpression e(c(1.0), std::make_unique<MultiplyExpression>(c(2.0), c(3.0)));
    auto copy = e.clone();
    CHECK(copy->toString() == e.toString());
    CHECK(copy->evaluate() == doctest::Approx(7.0));
}

TEST_CASE("Nested expression containing a variable cannot be evaluated") {
    AddExpression e(x(), c(1.0));
    CHECK(e.toString() == "(x + 1)");
    CHECK_THROWS_AS(e.evaluate(), std::runtime_error);
}

TEST_CASE("Function holds domain, codomain and rule") {
    auto domain = IntervalSet::all_real();
    auto codomain = IntervalSet::all_real();
    Function f("f", domain.get(), codomain.get(), std::make_unique<AddExpression>(x(), c(1.0)));
    CHECK(f.name == "f");
    CHECK(f.toString().find("f(x) = (x + 1)") != std::string::npos);
}
