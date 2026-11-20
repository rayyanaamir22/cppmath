// One parsed expression, evaluated over the reals and in F_p: only the interpretation changes.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include <memory>
#include <stdexcept>
#include "cppmath/abstract_algebra/field/FieldInterpretation.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::abstract_algebra::example;
using namespace cppmath::core;

namespace {
ExprPtr P(const std::string& s) { return parse::Parser().parse(s); }

int eval_mod(const Expression& e, const std::shared_ptr<const FiniteField>& f, int x) {
    Environment<FiniteFieldElement> env;
    env.bind("x", FiniteFieldElement(f, x));
    return evaluate(e, finite_field_interpretation(f), env).int_value();
}
}

TEST_CASE("polynomial in F_7") {
    auto f7 = std::make_shared<FiniteField>(7);
    auto e = P("x*x + 3*x + 1");
    CHECK(eval_mod(*e, f7, 3) == 5);   // 19 mod 7
    CHECK(eval_mod(*e, f7, 0) == 1);
    CHECK(eval_mod(*e, f7, 6) == 6);   // 55 mod 7
}

TEST_CASE("evaluating in F_p agrees with the reals reduced mod p (ring homomorphism)") {
    // No division: for + - * and integer numerals, reduce-then-compute == compute-then-reduce.
    for (int p : {2, 3, 5, 7, 13}) {
        auto f = std::make_shared<FiniteField>(p);
        for (const char* src : {"x*x + 3*x + 1", "(x + 5) * (x - 2)", "-x * x * x + 11", "100 - x*7"}) {
            auto e = P(src);
            for (int x = 0; x < p; ++x) {
                Environment<double> renv;
                renv.bind("x", x);
                double real = evaluate_real(*e, renv);
                int expected = static_cast<int>(std::fmod(std::fmod(real, p) + p, p));
                INFO("p=" << p << " x=" << x << " expr=" << src);
                CHECK(eval_mod(*e, f, x) == expected);
            }
        }
    }
}

TEST_CASE("subtraction and negation wrap around") {
    auto f7 = std::make_shared<FiniteField>(7);
    CHECK(eval_mod(*P("2 - 5"), f7, 0) == 4);
    CHECK(eval_mod(*P("-x"), f7, 3) == 4);
    CHECK(eval_mod(*P("-x"), f7, 0) == 0);
    CHECK(eval_mod(*P("--x"), f7, 3) == 3);
}

TEST_CASE("division is multiplication by the inverse") {
    auto f7 = std::make_shared<FiniteField>(7);
    auto inv = P("1 / x");
    for (int x = 1; x < 7; ++x) CHECK((x * eval_mod(*inv, f7, x)) % 7 == 1);
    CHECK(eval_mod(*P("6 / 4"), f7, 0) == 5);                // 6 * 4^-1 = 6 * 2 = 12 = 5
    CHECK_THROWS_AS(eval_mod(*inv, f7, 0), std::invalid_argument);   // 1/0, as over the reals
}

TEST_CASE("numerals are reduced mod p and must be integers") {
    auto f7 = std::make_shared<FiniteField>(7);
    CHECK(eval_mod(*P("10"), f7, 0) == 3);
    CHECK(eval_mod(*P("1e3"), f7, 0) == 6);                  // 1000 = 142*7 + 6
    CHECK(eval_mod(*P("123456789012"), f7, 0) == 123456789012LL % 7);
    CHECK_THROWS_AS(eval_mod(*P("0.5"), f7, 0), std::invalid_argument);
}

TEST_CASE("operators with no meaning in F_p throw out_of_range") {
    auto f7 = std::make_shared<FiniteField>(7);
    CHECK_THROWS_AS(eval_mod(*P("sin(x)"), f7, 1), std::out_of_range);
    CHECK_THROWS_AS(eval_mod(*P("x + pi"), f7, 1), std::out_of_range);
    CHECK_THROWS_AS(eval_mod(*P("x ^ 2"), f7, 1), std::out_of_range);   // pow is deliberately undefined
}

TEST_CASE("binders have no meaning in F_p, and the error says so") {
    auto f7 = std::make_shared<FiniteField>(7);
    // sum's bounds would be field elements, and "1..10" is not meaningful there: it is left undefined, not guessed
    CHECK_THROWS_AS(eval_mod(*P("sum(i, 1, 3, i)"), f7, 0), std::out_of_range);
    CHECK_THROWS_WITH(eval_mod(*P("sum(i, 1, 3, i)"), f7, 0), doctest::Contains("'sum'"));
    CHECK_FALSE(finite_field_interpretation(f7).defines("sum"));
}

TEST_CASE("relations, logic and sets have no meaning in F_p") {
    auto f7 = std::make_shared<FiniteField>(7);
    // F_p has no order, and sets of integers are not sets of field elements: undefined, not guessed
    CHECK_THROWS_AS(eval_mod(*P("x < 3"), f7, 1), std::out_of_range);
    CHECK_THROWS_AS(eval_mod(*P("x = 3"), f7, 1), std::out_of_range);
    CHECK_THROWS_AS(eval_mod(*P("x in {1, 2}"), f7, 1), std::out_of_range);
    CHECK_THROWS_AS(eval_mod(*P("x in [0, 1)"), f7, 1), std::out_of_range);
}

TEST_CASE("unbound variables and a null field") {
    auto f7 = std::make_shared<FiniteField>(7);
    Environment<FiniteFieldElement> empty;
    CHECK_THROWS_AS(evaluate(*P("y + 1"), finite_field_interpretation(f7), empty), std::out_of_range);
    CHECK_THROWS_AS(finite_field_interpretation(nullptr), std::invalid_argument);
}

TEST_CASE("F_2: the smallest field") {
    auto f2 = std::make_shared<FiniteField>(2);
    CHECK(eval_mod(*P("x + x"), f2, 1) == 0);
    CHECK(eval_mod(*P("x * x + x"), f2, 1) == 0);
    CHECK(eval_mod(*P("1 / x"), f2, 1) == 1);
}
