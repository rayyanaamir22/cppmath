#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include "cppmath/calculus/diff.hpp"
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::core;
using cppmath::calculus::DerivativeRules;
using cppmath::calculus::RuleContext;
using cppmath::calculus::differentiate;

namespace {
ExprPtr P(const std::string& s) { return parse::Parser().parse(s); }
std::string D(const std::string& src, const std::string& var = "x") { return differentiate(*P(src), var)->toString(); }

double at(const Expression& e, double x) {
    Environment<double> env;
    env.bind("x", x);
    env.bind("y", 2.0);   // a second variable that is held constant
    return evaluate_real(e, env);
}
}

TEST_CASE("leaves") {
    CHECK(D("5") == "0");
    CHECK(D("x") == "1");
    CHECK(D("y") == "0");        // another variable is a constant
    CHECK(D("pi") == "0");
    CHECK(D("e") == "0");
    CHECK(D("y", "y") == "1");
}

TEST_CASE("sum, difference, negation, constant multiples") {
    CHECK(D("x + 1") == "1");
    CHECK(D("3 * x + 1") == "3");
    CHECK(D("x - y") == "1");
    CHECK(D("-x") == "(-1)");
    CHECK(D("y * x") == "y");
    CHECK(D("2 * 3 * x") == "6");
}

TEST_CASE("product, quotient, power rules") {
    CHECK(D("x * x") == "(x + x)");
    CHECK(D("x * sin(x)") == "(sin(x) + (x * cos(x)))");
    CHECK(D("1 / x") == "(-(1 / (x ^ 2)))");
    CHECK(D("x ^ 3") == "(3 * (x ^ 2))");
    CHECK(D("x ^ 2 + 3 * x + 1") == "((2 * x) + 3)");
    CHECK(D("2 ^ x") == "((2 ^ x) * log(2))");
    CHECK(D("x ^ y") == "(y * (x ^ (y - 1)))");   // y is a constant here
}

TEST_CASE("elementary functions and the chain rule") {
    CHECK(D("sin(x)") == "cos(x)");
    CHECK(D("cos(x)") == "(-sin(x))");
    CHECK(D("exp(x)") == "exp(x)");
    CHECK(D("log(x)") == "(1 / x)");
    CHECK(D("sqrt(x)") == "(1 / (2 * sqrt(x)))");
    CHECK(D("sin(x * x)") == "(cos((x * x)) * (x + x))");
    CHECK(D("exp(2 * x)") == "(exp((2 * x)) * 2)");
    CHECK(D("log(sin(x))") == "(cos(x) / sin(x))");
}

TEST_CASE("sums: differentiation goes under the binder") {
    CHECK(D("sum(i, 1, 3, i * x)") == "sum(i, 1, 3, i)");
    CHECK(D("sum(i, 1, 3, x ^ i)") == "sum(i, 1, 3, (i * (x ^ (i - 1))))");
    CHECK(D("sum(x, 1, 3, x)") == "0");                     // x is rebound inside: no dependence on the outer x
    CHECK(D("sum(i, 1, n, i * x)", "x") == "sum(i, 1, n, i)");   // other free variables in the bounds are fine
}

TEST_CASE("errors") {
    CHECK_THROWS_AS(D("sum(i, 1, x, i)"), std::invalid_argument);     // the bound depends on x
    CHECK_THROWS_AS(D("prod(i, 1, 3, i * x)"), std::invalid_argument);   // no rule for prod
    CHECK_THROWS_WITH(D("prod(i, 1, 3, i * x)"), doctest::Contains("prod"));
}

TEST_CASE("agrees with a numerical derivative on a range of functions") {
    struct Case { const char* src; double x0; };
    const Case cases[] = {
        {"x ^ 3 - 2 * x + 5", 1.3},        {"sin(x) * cos(x)", 0.4},          {"exp(x * x)", 0.3},
        {"log(x * x + 1)", 0.8},           {"sqrt(1 + x * x)", 1.1},          {"1 / (1 + x ^ 2)", 0.7},
        {"x ^ x", 1.5},                    {"2 ^ x", 0.5},                    {"x ^ (x + 1)", 0.9},
        {"tan(x)", 0.3},                   {"abs(x - 2)", 3.0},               {"abs(x - 2)", 1.0},
        {"sin(sin(x))", 0.9},              {"(x + 1) / (x - 1)", 3.0},        {"exp(sin(x)) * x", 0.2},
        {"sum(i, 1, 4, x ^ i)", 0.8},      {"sum(i, 1, 3, sin(i * x))", 0.5}, {"x * x * x * x", 2.0},
        {"pi * x + e", 1.0},               {"-x ^ 2", 1.0},                   {"(2 * x) ^ 3", 0.6},
        {"sqrt(x) * log(x)", 2.0},         {"y * x ^ 2 + y", 1.7},            {"(x ^ 2 + 1) ^ 3", 0.5},
    };
    for (const auto& c : cases) {
        INFO("f(x) = " << c.src << " at x = " << c.x0);
        auto f = P(c.src);
        auto df = differentiate(*f, "x");
        const double h = 1e-6;
        double numeric = (at(*f, c.x0 + h) - at(*f, c.x0 - h)) / (2 * h);
        double symbolic = at(*df, c.x0);
        CHECK(symbolic == doctest::Approx(numeric).epsilon(1e-5));
    }
}

TEST_CASE("second derivatives, by differentiating twice") {
    auto f = P("x ^ 4 + sin(x)");
    auto d2 = differentiate(*differentiate(*f, "x"), "x");
    for (double x0 : {0.5, 1.0, 2.0}) {
        const double h = 1e-4;
        double numeric = (at(*f, x0 + h) - 2 * at(*f, x0) + at(*f, x0 - h)) / (h * h);
        CHECK(at(*d2, x0) == doctest::Approx(numeric).epsilon(1e-4));
    }
    CHECK(differentiate(*P("x ^ 3"), "x")->toString() == "(3 * (x ^ 2))");
    CHECK(differentiate(*differentiate(*P("x ^ 3"), "x"), "x")->toString() == "(3 * (2 * x))");
}

TEST_CASE("the input is not modified, and unrelated terms vanish from the result") {
    auto f = P("x * y + sin(x)");
    std::string before = f->toString();
    auto df = differentiate(*f, "x");
    CHECK(f->toString() == before);
    CHECK(df->toString() == "(y + cos(x))");

    // a variable that only multiplies a constant term vanishes from the result
    auto g = differentiate(*P("z * 5 + x"), "x");
    CHECK(g->toString() == "1");
    CHECK(free_variables(*g).empty());
}

TEST_CASE("custom derivative rules for a new operator") {
    Signature sig;
    sig.extend(Signature::standard());
    sig.add(Operator("sq", "sq", Arity::fixed(1), {Sort::Real()}, Sort::Real(), Notation::FunctionCall));
    auto f = parse::Parser(sig).parse("sq(x * y)");

    // without a rule, differentiation fails and says which operator
    CHECK_THROWS_AS(differentiate(*f, "x"), std::invalid_argument);
    CHECK_THROWS_WITH(differentiate(*f, "x"), doctest::Contains("'sq'"));

    DerivativeRules rules = DerivativeRules::standard();    // copy, then extend
    CHECK_FALSE(rules.defines("sq"));
    rules.define("sq", [](const RuleContext& c) {            // (u^2)' = 2 u u'
        return build::mul(build::mul(build::num(2), c.node.args()[0]->clone()), std::move(c.d[0]));
    });
    CHECK(rules.defines("sq"));
    CHECK(differentiate(*f, "x", rules)->toString() == "((2 * (x * y)) * y)");
    CHECK_FALSE(DerivativeRules::standard().defines("sq"));  // the shared table is untouched
}

TEST_CASE("rules table") {
    const auto& std_rules = DerivativeRules::standard();
    for (const char* op : {"add", "sub", "mul", "div", "pow", "neg", "sin", "cos", "tan", "exp", "log", "sqrt", "abs", "pi", "e"})
        CHECK(std_rules.defines(op));
    CHECK_THROWS_AS(std_rules.get("nonsense"), std::invalid_argument);
}
