// Binders: Bind nodes, scoping, capture-avoiding substitution, evaluation and parsing of sum/prod.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Bind.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Fold.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/Substitution.hpp"
#include "cppmath/core/parse/ParseError.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;

namespace {
ExprPtr P(const std::string& s) { return parse::Parser().parse(s); }
double eval(const std::string& s) { return evaluate_real(*P(s)); }
std::set<std::string> names(std::initializer_list<const char*> l) { return {l.begin(), l.end()}; }
}

// --------------------------------------------------------------- the node

TEST_CASE("Bind: construction and canonical text") {
    auto e = sum("i", num(1), num(3), pow(var("i"), num(2)));
    CHECK(e->toString() == "sum(i, 1, 3, (i ^ 2))");
    CHECK(e->sort() == Sort::Real());

    auto* b = dynamic_cast<const Bind*>(e.get());
    REQUIRE(b != nullptr);
    CHECK(b->op().name() == "sum");
    CHECK(b->variable().name() == "i");
    CHECK(b->operands().size() == 3);
    CHECK(b->body().toString() == "(i ^ 2)");

    CHECK(prod("k", num(1), num(4), var("k"))->toString() == "prod(k, 1, 4, k)");
}

TEST_CASE("Bind: validation") {
    const Signature& s = Signature::standard();
    auto three = [] {
        std::vector<ExprPtr> v;
        v.push_back(num(1)); v.push_back(num(2)); v.push_back(num(3));
        return v;
    };
    CHECK_NOTHROW(Bind(s.get("sum"), "i", three()));
    CHECK_THROWS_AS(Bind(s.get("add"), "i", three()), std::invalid_argument);   // not a binder
    CHECK_THROWS_AS(Bind(s.get("sum"), "1x", three()), std::invalid_argument);  // bad identifier
    CHECK_THROWS_AS(Bind(s.get("sum"), "", three()), std::invalid_argument);

    std::vector<ExprPtr> two;
    two.push_back(num(1)); two.push_back(num(2));
    CHECK_THROWS_AS(Bind(s.get("sum"), "i", std::move(two)), std::invalid_argument);   // missing the body

    std::vector<ExprPtr> with_null = three();
    with_null[1] = nullptr;
    CHECK_THROWS_AS(Bind(s.get("sum"), "i", std::move(with_null)), std::invalid_argument);

    // sort mismatch: a binder whose body must be Bool
    Signature custom;
    custom.add(Operator("forall", "forall", Arity::fixed(1), {Sort::Bool()}, Sort::Bool(), Notation::Binder, 0,
                        Assoc::Left, {}, Sort::Real()));
    std::vector<ExprPtr> real_body;
    real_body.push_back(num(1));
    CHECK_THROWS_AS(Bind(custom.get("forall"), "x", std::move(real_body)), std::invalid_argument);
}

TEST_CASE("builders reject reserved words as bound or free variables") {
    for (const char* w : {"pi", "e", "sin", "sqrt", "sum", "prod"}) {
        INFO("word: " << w);
        CHECK_THROWS_AS(var(w), std::invalid_argument);
        CHECK_THROWS_AS(sum(w, num(1), num(2), num(3)), std::invalid_argument);
    }
    CHECK_NOTHROW(var("summ"));
    CHECK_NOTHROW(var("i"));
}

TEST_CASE("Bind: clone, equals and hash are structural") {
    auto e = sum("i", num(1), var("n"), mul(var("i"), var("x")));
    auto copy = e->clone();
    CHECK(copy.get() != e.get());
    CHECK(copy->equals(*e));
    CHECK(copy->hash() == e->hash());
    CHECK(copy->toString() == e->toString());

    CHECK_FALSE(e->equals(*sum("j", num(1), var("n"), mul(var("j"), var("x")))));   // renamed variable
    CHECK_FALSE(e->equals(*sum("i", num(2), var("n"), mul(var("i"), var("x")))));   // different bound
    CHECK_FALSE(e->equals(*prod("i", num(1), var("n"), mul(var("i"), var("x")))));  // different binder
    CHECK_FALSE(e->equals(*var("x")));
    CHECK_FALSE(var("x")->equals(*e));
}

// --------------------------------------------------------------- analysis

TEST_CASE("free_variables respects scope") {
    CHECK(free_variables(*P("sum(i, 1, n, i * x)")) == names({"n", "x"}));
    CHECK(free_variables(*P("sum(i, 1, 3, sum(j, 1, i, i * j))")).empty());
    CHECK(free_variables(*P("sum(i, 1, 3, i) + i")) == names({"i"}));        // the second i is outside
    CHECK(free_variables(*P("sum(i, 1, i, i)")) == names({"i"}));            // the upper bound is outside the scope
    CHECK(contains_variable(*P("sum(i, 1, 3, i * x)"), "x"));
    CHECK_FALSE(contains_variable(*P("sum(i, 1, 3, i * x)"), "i"));
}

TEST_CASE("node_count and tree_depth with binders") {
    auto e = P("sum(i, 1, 3, i)");
    CHECK(node_count(*e) == 4);    // the bind, 1, 3, and the body i (the bound name is not a node)
    CHECK(tree_depth(*e) == 2);
    CHECK(tree_depth(*P("sum(i, 1, 3, sum(j, 1, i, i * j))")) == 4);
}

TEST_CASE("alpha_equivalent: equal up to renaming of bound variables") {
    CHECK(alpha_equivalent(*P("sum(i, 1, 3, i)"), *P("sum(j, 1, 3, j)")));
    CHECK(alpha_equivalent(*P("sum(i, 1, 3, i * x)"), *P("sum(k, 1, 3, k * x)")));
    CHECK(alpha_equivalent(*P("sum(i, 1, 3, sum(j, 1, i, i * j))"), *P("sum(a, 1, 3, sum(b, 1, a, a * b))")));
    CHECK(alpha_equivalent(*P("x + 1"), *P("x + 1")));

    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, 3, i * x)"), *P("sum(i, 1, 3, i * y)")));     // free names differ
    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, 3, i)"), *P("sum(j, 1, 3, i)")));             // i is free on the right
    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, 3, sum(j, 1, 3, i))"), *P("sum(i, 1, 3, sum(j, 1, 3, j))")));
    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, 3, i)"), *P("prod(i, 1, 3, i)")));
    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, i, i)"), *P("sum(j, 1, j, j)")));             // the bound's i is free, j is not
    CHECK_FALSE(alpha_equivalent(*P("sum(i, 1, 3, i)"), *P("1 + 3")));
    // it is a coarser relation than equals: syntactic equality implies it
    CHECK(alpha_equivalent(*P("sum(i, 1, 3, i)"), *P("sum(i, 1, 3, i)")));
}

// --------------------------------------------------------------- fold / transform

TEST_CASE("fold with an on_bind handler") {
    auto binders = [](const Expression& e) {
        return fold<int>(e,
                         [](const Constant&) { return 0; },
                         [](const Variable&) { return 0; },
                         [](const Operator&, std::vector<int> kids) { int n = 0; for (int k : kids) n += k; return n; },
                         [](const Bind&, std::vector<int> kids) { int n = 1; for (int k : kids) n += k; return n; });
    };
    CHECK(binders(*P("x + 1")) == 0);
    CHECK(binders(*P("sum(i, 1, 3, i)")) == 1);
    CHECK(binders(*P("sum(i, 1, 3, prod(j, 1, i, j)) + sum(k, 1, 2, k)")) == 3);
}

TEST_CASE("the 3-handler fold rejects binders with a clear error") {
    auto count = [](const Expression& e) {
        return fold<int>(e,
                         [](const Constant&) { return 1; },
                         [](const Variable&) { return 1; },
                         [](const Operator&, std::vector<int> kids) { int n = 1; for (int k : kids) n += k; return n; });
    };
    CHECK(count(*P("x + 1")) == 3);
    CHECK_THROWS_AS(count(*P("sum(i, 1, 3, i)")), std::invalid_argument);
    CHECK_THROWS_WITH(count(*P("sum(i, 1, 3, i)")), doctest::Contains("on_bind"));
}

TEST_CASE("transform rebuilds binders and is not scope-aware") {
    auto e = P("sum(i, 1, 3, i * x)");
    auto same = transform(*e, [](ExprPtr n) { return n; });
    CHECK(same->equals(*e));
    CHECK(same.get() != e.get());

    // every Variable is renamed, including the bound i and its uses: the rewrite sees plain nodes
    auto renamed = transform(*e, [](ExprPtr n) -> ExprPtr {
        auto* v = dynamic_cast<const Variable*>(n.get());
        return v && v->name() == "i" ? var("k") : std::move(n);
    });
    CHECK(renamed->toString() == "sum(i, 1, 3, (k * x))");   // the binder's own name is not a node
}

// --------------------------------------------------------------- substitution

TEST_CASE("substitution replaces free variables in bounds and body") {
    Substitution s;
    s.bind("n", *num(5));
    CHECK(s.apply(*P("sum(i, 1, n, i * n)"))->toString() == "sum(i, 1, 5, (i * 5))");
}

TEST_CASE("substitution respects shadowing") {
    Substitution s;
    s.bind("i", *num(9));
    // the bound i is not replaced inside the body; the x there is untouched too
    CHECK(s.apply(*P("sum(i, 1, 3, i + x)"))->toString() == "sum(i, 1, 3, (i + x))");
    // ... but the bounds are outside the scope, so an i there is free and is replaced
    CHECK(s.apply(*P("sum(i, i, 3, i)"))->toString() == "sum(i, 9, 3, i)");
    // a free i outside the binder is replaced as usual
    CHECK(s.apply(*P("sum(i, 1, 3, i) + i"))->toString() == "(sum(i, 1, 3, i) + 9)");
}

TEST_CASE("substitution avoids capture by renaming the bound variable") {
    Substitution s;
    s.bind("x", *var("i"));   // would be captured by the binder i
    auto r = s.apply(*P("sum(i, 1, 3, i + x)"));
    CHECK(r->toString() == "sum(i_1, 1, 3, (i_1 + i))");
    CHECK(free_variables(*r) == names({"i"}));   // the substituted i is still free
}

TEST_CASE("substitution renames only when capture would happen") {
    Substitution a;
    a.bind("x", *var("y"));
    CHECK(a.apply(*P("sum(i, 1, 3, i + x)"))->toString() == "sum(i, 1, 3, (i + y))");   // y can't be captured

    Substitution b;
    b.bind("x", *var("i"));
    CHECK(b.apply(*P("sum(i, 1, 3, i)"))->toString() == "sum(i, 1, 3, i)");   // x does not occur free in the body
}

TEST_CASE("fresh names avoid every name in sight") {
    Substitution s;
    s.bind("x", *var("i"));
    // i_1 is already a free variable of the body, so the fresh name must be i_2
    auto r = s.apply(*P("sum(i, 1, 3, (i + x) + i_1)"));
    CHECK(r->toString() == "sum(i_2, 1, 3, ((i_2 + i) + i_1))");
    CHECK(free_variables(*r) == names({"i", "i_1"}));
}

TEST_CASE("capture avoidance in nested binders") {
    Substitution s;
    s.bind("x", *var("j"));
    auto r = s.apply(*P("sum(i, 1, 3, sum(j, 1, 3, i + x))"));
    CHECK(r->toString() == "sum(i, 1, 3, sum(j_1, 1, 3, (i + j)))");
}

TEST_CASE("substitution is simultaneous across a binder") {
    Substitution swap;
    swap.bind("x", *var("y"));
    swap.bind("y", *var("x"));
    CHECK(swap.apply(*P("sum(i, 1, 3, i * x + y)"))->toString() == "sum(i, 1, 3, ((i * y) + x))");
}

TEST_CASE("substitution preserves meaning: the substitution lemma") {
    // Evaluating e[x := r] with the free variables of r set equals evaluating e with x set to r's value.
    auto e = P("sum(i, 1, 4, i * x + y)");
    Substitution s;
    s.bind("x", *P("i * 2"));   // mentions i: a naive substitution would be captured
    s.bind("y", *P("i"));
    auto replaced = s.apply(*e);

    Environment<double> outer;           // i = 10, so x = 20 and y = 10
    outer.bind("i", 10);
    Environment<double> direct;
    direct.bind("x", 20);
    direct.bind("y", 10);
    CHECK(evaluate_real(*replaced, outer) == doctest::Approx(evaluate_real(*e, direct)));
    CHECK(evaluate_real(*e, direct) == doctest::Approx(240.0));   // 20*(1+2+3+4) + 4*10
}

TEST_CASE("compose with binders holds up to renaming") {
    Substitution s, t;
    s.bind("x", *P("i + 1"));   // captured by the binder when applied
    t.bind("i", *num(5));
    auto e = P("sum(i, 1, 2, i * x)");

    auto sequential = t.apply(*s.apply(*e));
    auto composed = compose(s, t).apply(*e);
    CHECK(alpha_equivalent(*composed, *sequential));
    CHECK(composed->toString() == "sum(i, 1, 2, (i * (5 + 1)))");
    CHECK(sequential->toString() == "sum(i_1, 1, 2, (i_1 * (5 + 1)))");   // the same thing, named differently
    CHECK_FALSE(composed->equals(*sequential));                           // equals() is syntactic
}

// --------------------------------------------------------------- evaluation

TEST_CASE("evaluate sum and prod over the reals") {
    CHECK(eval("sum(i, 1, 10, i)") == doctest::Approx(55.0));
    CHECK(eval("sum(i, 1, 4, i ^ 2)") == doctest::Approx(30.0));
    CHECK(eval("prod(i, 1, 5, i)") == doctest::Approx(120.0));
    CHECK(eval("sum(k, 3, 3, 7)") == doctest::Approx(7.0));          // a single term
    CHECK(eval("sum(i, 1, 100, 1)") == doctest::Approx(100.0));      // the body need not use i
    CHECK(eval("sum(i, -2, 2, i)") == doctest::Approx(0.0));         // negative bounds
}

TEST_CASE("empty ranges give the identity of the operation") {
    CHECK(eval("sum(i, 5, 1, i)") == 0.0);
    CHECK(eval("prod(i, 5, 1, i)") == 1.0);
}

TEST_CASE("nested binders and binders inside expressions") {
    CHECK(eval("sum(i, 1, 3, sum(j, 1, i, j))") == doctest::Approx(10.0));   // 1 + 3 + 6
    CHECK(eval("prod(i, 1, 5, i) / sum(i, 1, 4, i)") == doctest::Approx(12.0));
    CHECK(eval("2 * sum(i, 1, 3, i) + 1") == doctest::Approx(13.0));
    CHECK(eval("sum(i, 1, 3, i) ^ 2") == doctest::Approx(36.0));
}

TEST_CASE("scoping during evaluation") {
    Environment<double> env;
    env.bind("n", 4);
    env.bind("x", 100);
    // free variable in a bound, read from the environment
    CHECK(evaluate_real(*P("sum(i, 1, n, i)"), env) == doctest::Approx(10.0));
    // the bound x shadows the environment's x inside the body only
    CHECK(evaluate_real(*P("sum(x, 1, 3, x)"), env) == doctest::Approx(6.0));
    CHECK(evaluate_real(*P("sum(x, 1, 3, x) + x"), env) == doctest::Approx(106.0));
    // a bound's i is evaluated outside the binder's scope: it reads the environment's i
    env.bind("i", 3);
    CHECK(evaluate_real(*P("sum(i, 1, i, i)"), env) == doctest::Approx(6.0));
    // the environment is not modified
    CHECK(env.lookup("i") == 3);
    CHECK(env.lookup("x") == 100);
}

TEST_CASE("evaluation errors inside binders") {
    CHECK_THROWS_AS(eval("sum(i, 1, 3, y)"), std::out_of_range);            // unbound in the body
    CHECK_THROWS_AS(eval("sum(i, 1, n, i)"), std::out_of_range);            // unbound in a bound
    CHECK_THROWS_AS(eval("sum(i, 1, 3, 1 / (i - 2))"), std::invalid_argument);   // division by zero at i = 2
    CHECK_THROWS_AS(eval("sum(i, 0.5, 3, i)"), std::invalid_argument);      // non-integer bound
    CHECK_THROWS_AS(eval("sum(i, 1, 1/0, i)"), std::invalid_argument);
    CHECK_THROWS_AS(eval("sum(i, 1, exp(1000), i)"), std::invalid_argument);   // infinite bound
    CHECK_THROWS_AS(eval("sum(i, 1, 1e8, 1)"), std::invalid_argument);         // too many terms: refuses to spin
    CHECK(eval("sum(i, 1, 1e6, 1)") == doctest::Approx(1e6));                  // large but allowed
}

TEST_CASE("a binder with no meaning in an interpretation throws out_of_range") {
    Interpretation<double> partial([](double v) { return v; });
    partial.define("add", [](const std::vector<double>& a) { return a[0] + a[1]; });
    Environment<double> env;
    CHECK_THROWS_AS(evaluate<double>(*P("sum(i, 1, 3, i)"), partial, env), std::out_of_range);
    CHECK_FALSE(partial.defines("sum"));
    CHECK(real_interpretation().defines("sum"));
    CHECK(real_interpretation().defines("prod"));
}

TEST_CASE("define_binder works for another carrier: integers mod 7") {
    Interpretation<int> mod7([](double v) { return static_cast<int>(v) % 7; });
    mod7.define("mul", [](const std::vector<int>& a) { return (a[0] * a[1]) % 7; });
    mod7.define_binder("sum", [](const std::vector<int>& bounds, const std::function<int(const int&)>& body) {
        int acc = 0;
        for (int k = bounds[0]; k <= bounds[1]; ++k) acc = (acc + body(k)) % 7;
        return acc;
    });
    Environment<int> env;
    CHECK(evaluate<int>(*P("sum(i, 1, 6, i * i)"), mod7, env) == 91 % 7);   // 1+4+9+16+25+36 = 91
}

// --------------------------------------------------------------- parsing

TEST_CASE("parser: binders") {
    CHECK(P("sum(i, 1, 10, i ^ 2)")->equals(*sum("i", num(1), num(10), pow(var("i"), num(2)))));
    CHECK(P("prod(k, 1, n, k)")->equals(*prod("k", num(1), var("n"), var("k"))));
    CHECK(P("  sum ( i , 1 , 3 , i )  ")->toString() == "sum(i, 1, 3, i)");
    CHECK(P("sum(i, 1, 3, i + 1) * 2")->toString() == "(sum(i, 1, 3, (i + 1)) * 2)");
    CHECK(P("1 + sum(i, 1, 3, i)")->toString() == "(1 + sum(i, 1, 3, i))");
    CHECK(P("sum(i, 1, 3, prod(j, 1, i, j))")->toString() == "sum(i, 1, 3, prod(j, 1, i, j))");
    CHECK(P("-sum(i, 1, 3, i)")->toString() == "(-sum(i, 1, 3, i))");
    CHECK(P("sum(i, 1, 3, sin(i))")->toString() == "sum(i, 1, 3, sin(i))");
}

TEST_CASE("parser: binder errors") {
    auto column_of_error = [](const std::string& src) -> std::size_t {
        try { P(src); } catch (const parse::ParseError& e) { return e.span().column; }
        return 0;
    };
    CHECK(column_of_error("sum(1, 1, 3, 1)") == 5);        // the bound variable must be a name
    CHECK(column_of_error("sum(pi, 1, 3, 1)") == 5);       // reserved word
    CHECK(column_of_error("sum(sin, 1, 3, 1)") == 5);
    CHECK(column_of_error("sum(i 1, 3, 4)") == 7);          // missing comma after the variable
    CHECK(column_of_error("sum(i, 1, 3)") == 1);            // operand count wrong: reported at the binder
    CHECK(column_of_error("sum(i, 1, 3, 4, 5)") == 1);
    CHECK(column_of_error("sum(i)") == 6);            // at the ')' where a ',' was expected
    CHECK(column_of_error("sum(i, 1, 3, i") == 15);         // unclosed: end of input
    CHECK(column_of_error("sum") == 1);                     // a binder name needs its arguments
    CHECK(column_of_error("1 + sum") == 5);
    CHECK_THROWS_AS(P("sum(i, 1, 3, i) i"), parse::ParseError);
}

TEST_CASE("round trip: parse(print(e)) equals e for trees with binders") {
    std::vector<ExprPtr> corpus;
    corpus.push_back(sum("i", num(1), num(10), var("i")));
    corpus.push_back(prod("k", num(1), var("n"), add(var("k"), num(1))));
    corpus.push_back(sum("i", num(1), num(3), sum("j", num(1), var("i"), mul(var("i"), var("j")))));
    corpus.push_back(add(sum("i", num(1), num(3), var("i")), var("i")));
    corpus.push_back(neg(prod("i", num(-2), num(2), sin(var("i")))));
    corpus.push_back(div(sum("i", num(0.5), num(1e20), var("i")), prod("j", sub(var("a"), num(1)), var("b"), var("j"))));
    corpus.push_back(sum("i_1", num(1), num(2), var("i_1")));
    for (const auto& e : corpus) {
        INFO("expression: " << e->toString());
        auto back = P(e->toString());
        CHECK(back->equals(*e));
        CHECK(back->toString() == e->toString());
    }
}
