#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/Simplify.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::core;

namespace {
ExprPtr P(const std::string& s) { return parse::Parser().parse(s); }
std::string S(const std::string& s) { return simplify(*P(s))->toString(); }
}

TEST_CASE("additive and multiplicative identities") {
    CHECK(S("x + 0") == "x");
    CHECK(S("0 + x") == "x");
    CHECK(S("x - 0") == "x");
    CHECK(S("0 - x") == "(-x)");
    CHECK(S("x * 1") == "x");
    CHECK(S("1 * x") == "x");
    CHECK(S("x * 0") == "0");
    CHECK(S("0 * x") == "0");
    CHECK(S("x * (-1)") == "(-x)");
    CHECK(S("x / 1") == "x");
    CHECK(S("x / (-1)") == "(-x)");
    CHECK(S("x - x") == "0");
    CHECK(S("(x + 1) - (x + 1)") == "0");
}

TEST_CASE("powers") {
    CHECK(S("x ^ 1") == "x");
    CHECK(S("x ^ 0") == "1");
    CHECK(S("1 ^ x") == "1");
    CHECK(S("0 ^ 2") == "0");
    CHECK(S("x ^ 2") == "(x ^ 2)");       // untouched
    CHECK(S("2 ^ 10") == "1024");
    CHECK(S("2 ^ (-1)") == "(2 ^ (-1))"); // not folded: would be inexact
    CHECK(S("0 ^ (-1)") == "(0 ^ (-1))"); // undefined, left alone
}

TEST_CASE("signs are pulled outward and cancelled") {
    CHECK(S("--x") == "x");
    CHECK(S("---x") == "(-x)");
    CHECK(S("x + (-y)") == "(x - y)");
    CHECK(S("(-x) + y") == "(y - x)");
    CHECK(S("x - (-y)") == "(x + y)");
    CHECK(S("(-x) * y") == "(-(x * y))");
    CHECK(S("x * (-y)") == "(-(x * y))");
    CHECK(S("(-x) * (-y)") == "(x * y)");
    CHECK(S("(-x) / y") == "(-(x / y))");
    CHECK(S("x / (-y)") == "(-(x / y))");
    CHECK(S("(-x) / (-y)") == "(x / y)");
}

TEST_CASE("constant folding") {
    CHECK(S("2 + 3") == "5");
    CHECK(S("2 - 5") == "(-3)");           // negative numbers are neg(literal)
    CHECK(S("2 * 3 * 4") == "24");
    CHECK(S("6 / 3") == "2");
    CHECK(S("-(-3)") == "3");
    CHECK(S("-0") == "0");
    CHECK(S("(-2) * (-3)") == "6");
    CHECK(S("1 + 2 + x") == "(3 + x)");
    CHECK(S("x + 1 + 2") == "((x + 1) + 2)");   // no reassociation
}

TEST_CASE("folding is exact: inexact and undefined results are left symbolic") {
    CHECK(S("1 / 3") == "(1 / 3)");
    CHECK(S("7 / 2") == "(7 / 2)");
    CHECK(S("1 / 0") == "(1 / 0)");
    CHECK(S("0 / 0") == "(0 / 0)");
    CHECK(S("sin(1)") == "sin(1)");
    CHECK(S("exp(1)") == "exp(1)");
    CHECK(S("1e308 * 10") == "(1e+308 * 10)");   // would overflow to infinity
}

TEST_CASE("elementary functions at exact points") {
    CHECK(S("sin(0)") == "0");
    CHECK(S("tan(0)") == "0");
    CHECK(S("cos(0)") == "1");
    CHECK(S("exp(0)") == "1");
    CHECK(S("log(1)") == "0");
    CHECK(S("sqrt(0)") == "0");
    CHECK(S("sqrt(1)") == "1");
    CHECK(S("abs(-3)") == "3");
    CHECK(S("sin(x - x)") == "0");            // the argument simplifies first
    CHECK(S("exp(x * 0)") == "1");
}

TEST_CASE("nested clean-up works bottom-up") {
    CHECK(S("(x + 0) * (1 * y) + 0 * z") == "(x * y)");
    CHECK(S("((x * 1) + (0 * y)) ^ 1") == "x");
    CHECK(S("sin(x + 0) * 1 + cos(0 * x)") == "(sin(x) + 1)");
    CHECK(S("((1 * x) + (x * 1)) + ((0 * x) + (3 * 1))") == "((x + x) + 3)");
}

TEST_CASE("binders: operands are simplified and the binder is kept") {
    CHECK(S("sum(i, 1, 2 + 1, i * 1)") == "sum(i, 1, 3, i)");
    CHECK(S("prod(i, 0 + 1, n, i + 0)") == "prod(i, 1, n, i)");
    CHECK(S("sum(i, 1, 3, 0 * i)") == "sum(i, 1, 3, 0)");   // the binder stays even if the body is constant
}

TEST_CASE("the input is not modified, and the result is a new tree") {
    auto e = P("(x + 0) * 1");
    std::string before = e->toString();
    auto s = simplify(*e);
    CHECK(e->toString() == before);
    CHECK(s->toString() == "x");
    auto already = P("x + y");
    CHECK(simplify(*already)->equals(*already));
    CHECK(simplify(*already).get() != already.get());
}

TEST_CASE("simplify is idempotent and preserves the value") {
    const char* corpus[] = {
        "(x + 0) * (1 * y) + 0 * z",         "x - (-y) + (-x) * (-y)",    "2 * 3 + x * (4 - 4)",
        "(-x) / (-y) + x / 1",               "x ^ 1 + x ^ 0 + 1 ^ x",     "-(-(-x)) + 0 - (-y)",
        "sin(0) + cos(0) * exp(0) - log(1)", "(x + 1) * (x - 1) - (x * x - 1)",
        "sum(i, 1, 3 + 1, i * 1 + 0) * (2 - 2 + x)", "x * (-1) + (-1) * y + (-2) * (-x)",
        "((1 + 2) * x) ^ (3 - 2)",           "abs(-3) * x - sqrt(1) * y",
    };
    for (const char* src : corpus) {
        INFO("expression: " << src);
        auto e = P(src);
        auto once = simplify(*e);
        CHECK(simplify(*once)->equals(*once));   // idempotent
        for (double x : {-1.5, 0.0, 0.7, 3.0}) {
            for (double y : {-2.0, 0.5, 4.0}) {
                Environment<double> env;
                env.bind("x", x);
                env.bind("y", y);
                env.bind("z", 1.0);
                INFO("x=" << x << " y=" << y);
                CHECK(evaluate_real(*once, env) == doctest::Approx(evaluate_real(*e, env)).epsilon(1e-12));
            }
        }
    }
}

TEST_CASE("simplify removes sub-expressions that would have failed (documented trade-off)") {
    // 0 * log(0) is an error to evaluate, but simplifies to 0: the identities are assumed to hold.
    auto e = P("0 * log(0)");
    CHECK_THROWS_AS(evaluate_real(*e), std::invalid_argument);
    CHECK(evaluate_real(*simplify(*e)) == 0.0);
}
