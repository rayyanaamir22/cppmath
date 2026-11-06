#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <numeric>
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/Fold.hpp"
#include "cppmath/core/Substitution.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;

namespace {
// (x + 2) * sin(y)
ExprPtr sample() { return mul(add(var("x"), num(2)), sin(var("y"))); }
}

TEST_CASE("fold: count constants") {
    auto n = fold<int>(*sample(),
                       [](const Constant&) { return 1; },
                       [](const Variable&) { return 0; },
                       [](const Operator&, std::vector<int> kids) { return std::accumulate(kids.begin(), kids.end(), 0); });
    CHECK(n == 1);
}

TEST_CASE("fold sees the operator and children bottom-up") {
    auto s = fold<std::string>(*sample(),
                               [](const Constant& k) { return k.toString(); },
                               [](const Variable& v) { return v.name(); },
                               [](const Operator& op, std::vector<std::string> kids) {
                                   std::string out = op.name() + "[";
                                   for (auto& k : kids) out += k + ";";
                                   return out + "]";
                               });
    CHECK(s == "mul[add[x;2;];sin[y;];]");
}

TEST_CASE("analysis utilities") {
    auto e = sample();
    CHECK(free_variables(*e) == std::set<std::string>{"x", "y"});
    CHECK(contains_variable(*e, "x"));
    CHECK_FALSE(contains_variable(*e, "z"));
    CHECK(node_count(*e) == 6);   // mul, add, x, 2, sin, y
    CHECK(tree_depth(*e) == 3);
    CHECK(node_count(*num(1)) == 1);
    CHECK(tree_depth(*num(1)) == 1);
    CHECK(free_variables(*pi()).empty());
}

TEST_CASE("transform: bottom-up rewrite, input untouched") {
    // replace every constant by 0
    auto e = sample();
    auto zeroed = transform(*e, [](ExprPtr node) -> ExprPtr {
        if (dynamic_cast<const Constant*>(node.get())) return num(0);
        return node;
    });
    CHECK(zeroed->toString() == "((x + 0) * sin(y))");
    CHECK(e->toString() == "((x + 2) * sin(y))");
}

TEST_CASE("transform: identity rewrite gives an equal, distinct tree") {
    auto e = sample();
    auto same = transform(*e, [](ExprPtr n) { return n; });
    CHECK(same->equals(*e));
    CHECK(same.get() != e.get());
}

TEST_CASE("substitution replaces variables") {
    Substitution s;
    s.bind("x", *mul(num(3), var("z")));
    CHECK(s.binds("x"));
    CHECK_FALSE(s.binds("y"));
    CHECK(s.apply(*sample())->toString() == "(((3 * z) + 2) * sin(y))");
}

TEST_CASE("substitution is simultaneous") {
    Substitution swap;
    swap.bind("x", *var("y"));
    swap.bind("y", *var("x"));
    auto e = sub(var("x"), var("y"));
    CHECK(swap.apply(*e)->toString() == "(y - x)");

    Substitution grow;  // x -> x + 1 must not recurse forever
    grow.bind("x", *add(var("x"), num(1)));
    CHECK(grow.apply(*var("x"))->toString() == "(x + 1)");
}

TEST_CASE("substitution rebinding replaces the earlier binding; empty is identity") {
    Substitution s;
    s.bind("x", *num(1));
    s.bind("x", *num(2));
    CHECK(s.size() == 1);
    CHECK(s.apply(*var("x"))->toString() == "2");
    CHECK(Substitution().apply(*sample())->equals(*sample()));
}

TEST_CASE("substitution rejects sort-incorrect replacements") {
    Substitution s;
    Variable b("b", Sort::Bool());
    s.bind("x", b);   // x is Real, b is Bool
    CHECK_THROWS_AS(s.apply(*var("x")), std::invalid_argument);
}

TEST_CASE("compose(first, second) applies first, then second") {
    Substitution s, t;
    s.bind("x", *add(var("y"), num(1)));   // x -> y + 1
    t.bind("y", *num(5));                  // y -> 5
    t.bind("w", *num(7));                  // w -> 7  (not touched by s)
    Substitution st = compose(s, t);

    auto e = add(var("x"), add(var("y"), var("w")));
    CHECK(st.apply(*e)->equals(*t.apply(*s.apply(*e))));
    CHECK(st.apply(*e)->toString() == "((5 + 1) + (5 + 7))");
}
