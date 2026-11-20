#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include <stdexcept>
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Evaluate.hpp"
#include "cppmath/core/RealInterpretation.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;

TEST_CASE("Environment: bind, lookup, shadowing, parent scope") {
    Environment<double> outer;
    outer.bind("x", 1.0);
    outer.bind("y", 2.0);

    Environment<double> inner(&outer);
    inner.bind("x", 10.0);   // shadows outer x

    CHECK(inner.lookup("x") == 10.0);
    CHECK(inner.lookup("y") == 2.0);
    CHECK(outer.lookup("x") == 1.0);
    CHECK(inner.has("y"));
    CHECK_FALSE(outer.has("z"));
    CHECK_THROWS_AS(inner.lookup("z"), std::out_of_range);

    outer.bind("x", 5.0);    // rebinding replaces
    CHECK(outer.lookup("x") == 5.0);
}

TEST_CASE("evaluate_real: arithmetic") {
    CHECK(evaluate_real(*add(num(5), num(3))) == doctest::Approx(8.0));
    CHECK(evaluate_real(*mul(num(4), num(2))) == doctest::Approx(8.0));
    CHECK(evaluate_real(*pow(num(2), num(3))) == doctest::Approx(8.0));
    CHECK(evaluate_real(*sub(num(2), num(5))) == doctest::Approx(-3.0));
    CHECK(evaluate_real(*div(num(1), num(4))) == doctest::Approx(0.25));
    CHECK(evaluate_real(*neg(num(3))) == doctest::Approx(-3.0));
    CHECK(evaluate_real(*num(-3)) == doctest::Approx(-3.0));
}

TEST_CASE("evaluate_real: functions and constants") {
    CHECK(evaluate_real(*sin(num(0))) == doctest::Approx(0.0));
    CHECK(evaluate_real(*cos(num(0))) == doctest::Approx(1.0));
    CHECK(evaluate_real(*tan(num(0))) == doctest::Approx(0.0));
    CHECK(evaluate_real(*exp(num(1))) == doctest::Approx(M_E));
    CHECK(evaluate_real(*log(euler())) == doctest::Approx(1.0));
    CHECK(evaluate_real(*sqrt(num(9))) == doctest::Approx(3.0));
    CHECK(evaluate_real(*abs(num(-4))) == doctest::Approx(4.0));
    CHECK(evaluate_real(*cos(pi())) == doctest::Approx(-1.0));
}

TEST_CASE("evaluate_real: variables come from the environment") {
    auto e = add(mul(var("x"), var("x")), var("y"));   // x*x + y
    Environment<double> env;
    env.bind("x", 3.0);
    env.bind("y", 1.0);
    CHECK(evaluate_real(*e, env) == doctest::Approx(10.0));

    env.bind("x", 4.0);   // same tree, new answer
    CHECK(evaluate_real(*e, env) == doctest::Approx(17.0));
}

TEST_CASE("unbound variables throw out_of_range") {
    CHECK_THROWS_AS(evaluate_real(*var("x")), std::out_of_range);
    CHECK_THROWS_AS(evaluate_real(*add(var("x"), num(1))), std::out_of_range);
}

TEST_CASE("bad domains throw invalid_argument") {
    CHECK_THROWS_AS(evaluate_real(*div(num(1), num(0))), std::invalid_argument);
    CHECK_THROWS_AS(evaluate_real(*log(num(0))), std::invalid_argument);
    CHECK_THROWS_AS(evaluate_real(*log(num(-1))), std::invalid_argument);
    CHECK_THROWS_AS(evaluate_real(*sqrt(num(-1))), std::invalid_argument);
    CHECK_THROWS_AS(evaluate_real(*pow(num(-8), num(0.5))), std::invalid_argument);   // NaN result
}

TEST_CASE("an operator missing from an interpretation throws out_of_range") {
    Interpretation<double> partial([](double v) { return v; });
    partial.define("add", [](const std::vector<double>& a) { return a[0] + a[1]; });
    Environment<double> env;
    CHECK(evaluate<double>(*add(num(1), num(2)), partial, env) == doctest::Approx(3.0));
    CHECK_THROWS_AS(evaluate<double>(*mul(num(1), num(2)), partial, env), std::out_of_range);
    CHECK(partial.defines("add"));
    CHECK_FALSE(partial.defines("mul"));
}

TEST_CASE("one tree, two carriers: reals and integers mod 7") {
    // Same term evaluated in a different algebra: only the interpretation changes.
    using Z7 = int;
    Interpretation<Z7> mod7([](double v) { return static_cast<int>(v) % 7; });
    mod7.define("add", [](const std::vector<Z7>& a) { return (a[0] + a[1]) % 7; });
    mod7.define("mul", [](const std::vector<Z7>& a) { return (a[0] * a[1]) % 7; });
    mod7.define("neg", [](const std::vector<Z7>& a) { return (7 - a[0]) % 7; });

    auto e = add(mul(num(3), var("x")), num(5));   // 3x + 5
    Environment<Z7> zenv;
    zenv.bind("x", 4);
    CHECK(evaluate<Z7>(*e, mod7, zenv) == (3 * 4 + 5) % 7);   // 17 mod 7 = 3

    Environment<double> renv;
    renv.bind("x", 4.0);
    CHECK(evaluate_real(*e, renv) == doctest::Approx(17.0));
}
