// Milestone 5: relations, logic and sets in expressions: Value, bracket literals, aliases, and
// round-trips with the text that the sets library itself prints.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include "cppmath/calculus/diff.hpp"
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/Simplify.hpp"
#include "cppmath/core/Substitution.hpp"
#include "cppmath/core/ValueInterpretation.hpp"
#include "cppmath/core/parse/ParseError.hpp"
#include "cppmath/core/parse/Parser.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/FiniteSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/UniversalSet.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;
namespace S = cppmath::sets;

namespace {
const parse::SortDeclarations SETS = {{"A", Sort::Set()}, {"B", Sort::Set()}, {"C", Sort::Set()}};
const parse::SortDeclarations BOOLS = {{"p", Sort::Bool()}, {"q", Sort::Bool()}};

ExprPtr P(const std::string& s, const parse::SortDeclarations& d = SETS) { return parse::Parser().parse(s, d); }
std::string text(const std::string& s, const parse::SortDeclarations& d = SETS) { return P(s, d)->toString(); }
Value V(const std::string& s) { return evaluate_value(*P(s), Environment<Value>()); }
bool truth(const std::string& s) { return V(s).as_bool(); }

ExprPtr setvar(const char* name) { return var(name, Sort::Set()); }
ExprPtr boolvar(const char* name) { return var(name, Sort::Bool()); }

std::size_t error_column(const std::string& s, const parse::SortDeclarations& d = SETS) {
    try { P(s, d); } catch (const parse::ParseError& e) { return e.span().column; }
    return 0;
}

// The same membership at a spread of points (for sets whose text differs after simplification).
void check_same_membership(const S::Set& original, const std::string& src) {
    INFO("set text: " << src);
    Value v = V(src);
    for (double x : {-3.0, -1.0, 0.0, 0.5, 1.0, 1.5, 2.0, 3.0, 5.0, 7.0})
        CHECK(v.as_set().contains(x) == original.contains(x));
}
}

// --------------------------------------------------------------- Value

TEST_CASE("Value: factories, sorts, checked accessors") {
    CHECK(Value::real(2.5).as_real() == 2.5);
    CHECK(Value::real(2.5).sort() == Sort::Real());
    CHECK(Value::boolean(true).as_bool());
    CHECK(Value::boolean(false).sort() == Sort::Bool());
    auto s = Value::set(S::IntervalSet::closed(0, 1));
    CHECK(s.sort() == Sort::Set());
    CHECK(s.as_set().contains(0.5));
    CHECK(s.is_set());
    CHECK_FALSE(s.is_real());

    CHECK_THROWS_AS(Value::real(1).as_bool(), std::invalid_argument);
    CHECK_THROWS_AS(Value::real(1).as_set(), std::invalid_argument);
    CHECK_THROWS_AS(Value::boolean(true).as_real(), std::invalid_argument);
    CHECK_THROWS_AS(s.as_real(), std::invalid_argument);
    CHECK_THROWS_WITH(s.as_real(), doctest::Contains("expected a Real, got a Set"));
    CHECK_THROWS_AS(Value::set(SetPtr()), std::invalid_argument);   // never null
}

TEST_CASE("Value: equality is per sort, and prints") {
    CHECK(Value::real(2).equals(Value::real(2)));
    CHECK_FALSE(Value::real(2).equals(Value::real(3)));
    CHECK_FALSE(Value::real(1).equals(Value::boolean(true)));     // different sorts are never equal
    CHECK(Value::set(S::IntervalSet::closed(0, 1)).equals(Value::set(S::IntervalSet::closed(0, 1))));
    CHECK_FALSE(Value::set(S::IntervalSet::closed(0, 1)).equals(Value::set(S::IntervalSet::open(0, 1))));

    CHECK(Value::real(0.1).toString() == "0.1");
    CHECK(Value::boolean(true).toString() == "true");
    CHECK(Value::set(S::FiniteSet({1, 2}).clone()).toString() == "{1, 2}");
}

// --------------------------------------------------------------- the vocabulary

TEST_CASE("signature: relations, logic and set operators") {
    const Signature& s = Signature::standard();
    CHECK(s.find_infix("<")->output_sort() == Sort::Bool());
    CHECK(s.find_infix("in")->output_sort() == Sort::Bool());
    CHECK(s.find_infix("U")->output_sort() == Sort::Set());
    CHECK(s.find_infix("∩")->name() == "intersection");
    CHECK(s.find_prefix("not")->output_sort() == Sort::Bool());
    CHECK(s.find_function("complement")->output_sort() == Sort::Set());
    CHECK(s.find_atom("∅")->name() == "empty_set");
    CHECK(s.find_atom("ℝ")->name() == "universe");
    CHECK(s.find_atom("true")->output_sort() == Sort::Bool());
    CHECK(s.find_atom("inf")->output_sort() == Sort::Real());

    // loosest to tightest: or, and, not, relations, U, ∩, then arithmetic
    int or_p = s.get("or").precedence(), and_p = s.get("and").precedence(), not_p = s.get("not").precedence();
    int rel_p = s.get("lt").precedence(), un_p = s.get("union").precedence(), in_p = s.get("intersection").precedence();
    CHECK(or_p < and_p);
    CHECK(and_p < not_p);
    CHECK(not_p < rel_p);
    CHECK(rel_p == s.get("in").precedence());
    CHECK(rel_p == s.get("eq").precedence());
    CHECK(rel_p < un_p);
    CHECK(un_p < in_p);
    CHECK(in_p < s.get("add").precedence());
}

TEST_CASE("signature: the bracket operators") {
    const Signature& s = Signature::standard();
    CHECK(s.find_bracket('[', ')')->name() == "interval_co");
    CHECK(s.find_bracket('(', ']')->name() == "interval_oc");
    CHECK(s.find_bracket('[', ']')->name() == "interval_cc");
    CHECK(s.find_bracket('(', ')')->name() == "interval_oo");
    CHECK(s.find_bracket('{', '}')->name() == "finite_set");
    CHECK(s.find_bracket('{', ')') == nullptr);
    CHECK(s.get("finite_set").accepts_count(0));
    CHECK(s.get("finite_set").accepts_count(9));
    CHECK_FALSE(s.get("interval_co").accepts_count(3));
    CHECK(s.get("interval_co").notation() == Notation::Bracket);
}

TEST_CASE("signature: a bracket operator's symbol must be an opener followed by a closer") {
    const Sort R = Sort::Real();
    auto make = [&](const char* sym) {
        return Operator("b", sym, Arity::fixed(2), {R, R}, Sort::Set(), Notation::Bracket);
    };
    CHECK_NOTHROW(make("[)"));
    CHECK_NOTHROW(make("{}"));
    for (const char* bad : {"", "[", "[]]", "ab", "][", ")(", "<>"}) {
        INFO("symbol: '" << bad << "'");
        CHECK_THROWS_AS(make(bad), std::invalid_argument);
    }
}

TEST_CASE("signature: aliases are read but never printed") {
    const Signature& s = Signature::standard();
    CHECK(s.find_infix("∈") == s.find_infix("in"));
    CHECK(s.find_infix("∪") == s.find_infix("U"));
    CHECK(s.find_infix("≤") == s.find_infix("<="));
    CHECK(s.find_infix("≥") == s.find_infix(">="));
    CHECK(s.find_infix("≠") == s.find_infix("!="));
    CHECK(s.find_infix("==") == s.find_infix("="));
    CHECK(s.find_infix("∧") == s.find_infix("and"));
    CHECK(s.find_infix("∨") == s.find_infix("or"));
    CHECK(s.find_prefix("¬") == s.find_prefix("not"));
    CHECK(s.find_atom("emptyset") == s.find_atom("∅"));
    CHECK(s.find_atom("reals") == s.find_atom("ℝ"));
    CHECK(s.find_infix("∈")->symbol() == "in");   // the operator keeps its own symbol for printing
}

TEST_CASE("signature: add_alias validation, and extend carries aliases over") {
    Signature sig;
    sig.add(Operator("f", "f", Arity::fixed(1), {Sort::Real()}, Sort::Real(), Notation::FunctionCall));
    sig.add(Operator("g", "g", Arity::fixed(1), {Sort::Real()}, Sort::Real(), Notation::FunctionCall));
    sig.add_alias("ff", "f");
    CHECK(sig.find_function("ff") == sig.find_function("f"));
    CHECK_THROWS_AS(sig.add_alias("x", "nope"), std::invalid_argument);   // unknown operator
    CHECK_THROWS_AS(sig.add_alias("ff", "g"), std::invalid_argument);     // alias taken
    CHECK_THROWS_AS(sig.add_alias("f", "g"), std::invalid_argument);      // clashes with f's own symbol
    CHECK_THROWS_AS(sig.add_alias("", "g"), std::invalid_argument);

    Signature with_brackets;
    with_brackets.extend(Signature::standard());
    CHECK_THROWS_AS(with_brackets.add_alias("ic", "interval_co"), std::invalid_argument);   // brackets have no aliases

    Signature copy;
    copy.extend(sig);
    CHECK(copy.find_function("ff") == copy.find_function("f"));
    CHECK(copy.find_function("ff") != sig.find_function("f"));   // the copy's own operator

    Signature all;
    all.extend(Signature::standard());
    CHECK(all.find_infix("∈") == all.find_infix("in"));
    CHECK(all.find_atom("reals") == all.find_atom("ℝ"));
}

TEST_CASE("signature: reserved words and lexer symbols") {
    const Signature& s = Signature::standard();
    for (const char* w : {"U", "in", "and", "or", "not", "true", "false", "inf", "emptyset", "reals", "complement", "pi", "sum"})
        CHECK(s.is_reserved_word(w));
    for (const char* w : {"A", "B", "x", "Union", "inn", "android", "p", ""}) CHECK_FALSE(s.is_reserved_word(w));

    auto syms = s.operator_symbols();
    auto has = [&](const char* sym) { return std::find(syms.begin(), syms.end(), sym) != syms.end(); };
    for (const char* sym : {"+", "-", "∩", "∅", "ℝ", "∈", "≤", "<=", "==", "¬"}) CHECK(has(sym));
    for (const char* sym : {"U", "in", "and", "not", "true", "inf", "[)", "{}", "sin"}) CHECK_FALSE(has(sym));
    CHECK(std::count(syms.begin(), syms.end(), "-") == 1);   // infix and prefix minus: listed once
}

TEST_CASE("builders reject the new reserved words as variables") {
    for (const char* w : {"U", "in", "and", "or", "not", "true", "false", "inf", "complement"}) {
        INFO("word: " << w);
        CHECK_THROWS_AS(var(w), std::invalid_argument);
        CHECK_THROWS_AS(var(w, Sort::Set()), std::invalid_argument);
    }
    CHECK_NOTHROW(var("A", Sort::Set()));
    CHECK(var("A", Sort::Set())->sort() == Sort::Set());
}

// --------------------------------------------------------------- printing

TEST_CASE("printing: set literals, relations and logic") {
    CHECK(interval(num(0), num(1), true, false)->toString() == "[0, 1)");
    CHECK(interval(num(0), num(1), false, true)->toString() == "(0, 1]");
    CHECK(interval(num(0), num(1), true, true)->toString() == "[0, 1]");
    CHECK(interval(num(0), num(1), false, false)->toString() == "(0, 1)");
    CHECK(finite_set({})->toString() == "{}");
    std::vector<ExprPtr> two;
    two.push_back(num(1));
    two.push_back(add(var("x"), num(1)));
    CHECK(finite_set(std::move(two))->toString() == "{1, (x + 1)}");
    CHECK(empty_set()->toString() == "∅");
    CHECK(reals()->toString() == "ℝ");

    CHECK(set_union(setvar("A"), setvar("B"))->toString() == "(A U B)");
    CHECK(intersection(setvar("A"), setvar("B"))->toString() == "(A ∩ B)");
    CHECK(complement(setvar("A"))->toString() == "complement(A)");
    CHECK(in(var("x"), setvar("A"))->toString() == "(x in A)");

    CHECK(lt(var("x"), num(3))->toString() == "(x < 3)");
    CHECK(le(var("x"), num(3))->toString() == "(x <= 3)");
    CHECK(gt(var("x"), num(3))->toString() == "(x > 3)");
    CHECK(ge(var("x"), num(3))->toString() == "(x >= 3)");
    CHECK(eq(var("x"), num(3))->toString() == "(x = 3)");
    CHECK(ne(var("x"), num(3))->toString() == "(x != 3)");

    CHECK(logical_and(boolvar("p"), boolvar("q"))->toString() == "(p and q)");
    CHECK(logical_or(boolvar("p"), boolvar("q"))->toString() == "(p or q)");
    CHECK(logical_not(boolvar("p"))->toString() == "(not p)");   // a word operator keeps a space before its operand
    CHECK(logical_not(lt(var("x"), num(1)))->toString() == "(not (x < 1))");
    CHECK(boolean(true)->toString() == "true");
    CHECK(boolean(false)->toString() == "false");
    CHECK(infinity()->toString() == "inf");
    CHECK(neg(infinity())->toString() == "(-inf)");
}

TEST_CASE("printing: builders check sorts when the tree is built") {
    CHECK_THROWS_AS(add(num(1), boolean(true)), std::invalid_argument);                 // Bool where a Real is needed
    CHECK_THROWS_AS(logical_and(num(1), boolean(true)), std::invalid_argument);
    CHECK_THROWS_AS(in(var("x"), num(1)), std::invalid_argument);                        // a Real is not a Set
    CHECK_THROWS_AS(set_union(setvar("A"), num(1)), std::invalid_argument);
    CHECK_THROWS_AS(interval(setvar("A"), num(1), true, true), std::invalid_argument);
    std::vector<ExprPtr> bad;
    bad.push_back(boolean(true));
    CHECK_THROWS_AS(finite_set(std::move(bad)), std::invalid_argument);                  // elements are Reals
    CHECK_NOTHROW(eq(num(1), empty_set()));                                              // `=` accepts any sorts statically
}

// --------------------------------------------------------------- parsing

TEST_CASE("parser: the four interval forms") {
    CHECK(P("[0, 1]")->equals(*interval(num(0), num(1), true, true)));
    CHECK(P("[0, 1)")->equals(*interval(num(0), num(1), true, false)));
    CHECK(P("(0, 1]")->equals(*interval(num(0), num(1), false, true)));
    CHECK(P("(0, 1)")->equals(*interval(num(0), num(1), false, false)));
    CHECK(text("  [ 0 ,1 )  ") == "[0, 1)");
}

TEST_CASE("parser: a comma after the first expression decides between an interval and a grouping") {
    CHECK(text("(1 + 2) * 3") == "((1 + 2) * 3)");           // plain grouping is unaffected
    CHECK(text("((1 + 2))") == "(1 + 2)");
    CHECK(text("(1, 2)") == "(1, 2)");                       // an interval
    CHECK(text("((1, 2))") == "(1, 2)");                     // an interval inside a grouping
    CHECK(text("(1, 2) U (3, 4]") == "((1, 2) U (3, 4])");
    CHECK(text("(0, 1) ∩ [0, 5]") == "((0, 1) ∩ [0, 5])");
}

TEST_CASE("parser: endpoints and elements are expressions") {
    CHECK(text("[a + 1, 2 * b)") == "[(a + 1), (2 * b))");
    CHECK(text("[sin(0), sum(i, 1, 3, i)]") == "[sin(0), sum(i, 1, 3, i)]");
    CHECK(text("{}") == "{}");
    CHECK(text("{ 1 , 2 ,3 }") == "{1, 2, 3}");
    CHECK(text("{1 + 1, x, -2}") == "{(1 + 1), x, (-2)}");
    CHECK(P("{}")->sort() == Sort::Set());
}

TEST_CASE("parser: sets and the real line as symbols and words") {
    CHECK(text("∅") == "∅");
    CHECK(text("ℝ") == "ℝ");
    CHECK(text("emptyset") == "∅");
    CHECK(text("reals") == "ℝ");
    CHECK(text("∅ U ℝ") == "(∅ U ℝ)");
    CHECK(text("complement(∅)") == "complement(∅)");
    CHECK(text("true") == "true");
    CHECK(text("x < inf") == "(x < inf)");
    CHECK(text("-inf") == "(-inf)");
}

TEST_CASE("parser: precedence of set operators, relations and logic") {
    CHECK(text("A U B U C") == "((A U B) U C)");
    CHECK(text("A U B ∩ C") == "(A U (B ∩ C))");            // ∩ binds tighter than U
    CHECK(text("A ∩ B U C") == "((A ∩ B) U C)");
    CHECK(text("x in A U B") == "(x in (A U B))");          // U binds tighter than `in`
    CHECK(text("x + 1 in A") == "((x + 1) in A)");          // arithmetic tighter than `in`
    CHECK(text("x * 2 < y + 1") == "((x * 2) < (y + 1))");
    CHECK(text("x < 3 and y > 2 or z = 0") == "(((x < 3) and (y > 2)) or (z = 0))");
    CHECK(text("x < 3 or y > 2 and z = 0") == "((x < 3) or ((y > 2) and (z = 0)))");
    CHECK(text("not x < 3 and y > 2") == "((not (x < 3)) and (y > 2))");   // not binds below relations, above and
    CHECK(text("not not true") == "(not (not true))");
    CHECK(text("x in A and not y in B") == "((x in A) and (not (y in B)))");
    CHECK(text("A = B and x != 1") == "((A = B) and (x != 1))");
}

TEST_CASE("parser: unicode and ascii aliases read as the canonical operators") {
    CHECK(text("x ∈ A ∪ B") == "(x in (A U B))");
    CHECK(text("x ≤ 3") == "(x <= 3)");
    CHECK(text("x≥3") == "(x >= 3)");                        // no spaces needed around symbols
    CHECK(text("x ≠ 3") == "(x != 3)");
    CHECK(text("x == 3") == "(x = 3)");
    CHECK(text("p ∧ q ∨ ¬p", BOOLS) == "((p and q) or (not p))");
    CHECK(text("¬p", BOOLS) == "(not p)");
}

TEST_CASE("parser: declared sorts decide whether a name is a Real, a Bool or a Set") {
    CHECK(P("A")->sort() == Sort::Set());
    CHECK(P("x")->sort() == Sort::Real());                   // not declared: a Real
    CHECK(P("p", BOOLS)->sort() == Sort::Bool());
    CHECK(P("x in A")->sort() == Sort::Bool());
    CHECK(P("A U B")->sort() == Sort::Set());

    // Without the declaration A and B are Reals, and a union of Reals is ill-sorted
    CHECK_THROWS_AS(parse::Parser().parse("A U B"), parse::ParseError);
    CHECK_THROWS_AS(parse::Parser().parse("x in A"), parse::ParseError);
    CHECK(error_column("A U B", {}) == 3);                   // reported at the operator
    CHECK(parse::Parser().parse("x < 3")->sort() == Sort::Bool());   // no declaration needed for Real-only text
}

TEST_CASE("parser: sort errors are positioned ParseErrors") {
    CHECK(error_column("1 + true") == 3);                    // a Bool is not a Real
    CHECK(error_column("not 1") == 1);                       // `not` needs a Bool
    CHECK(error_column("x in 1") == 3);                      // `in` needs a Set on the right
    CHECK(error_column("3 and 4") == 3);
    CHECK(error_column("{true}") == 1);                      // set elements are Reals
    CHECK(error_column("{ {1} }") == 1);                     // reported at the outer brace: a set is not a Real, so sets can't nest
    CHECK(error_column("[A, 1)") == 1);
    CHECK(error_column("A < B") == 3);
    CHECK(error_column("complement(1)") == 1);
}

TEST_CASE("parser: malformed bracket literals") {
    CHECK(error_column("[1)") == 3);                         // an interval needs two endpoints
    CHECK(error_column("[1, 2") == 6);                       // unclosed: reported at the end of input
    CHECK(error_column("(1, 2, 3)") == 6);                   // three endpoints
    CHECK(error_column("(1, 2}") == 6);                      // wrong closing delimiter
    CHECK(error_column("[1, 2}") == 6);
    CHECK(error_column("{1, 2") == 6);
    CHECK(error_column("{1, 2)") == 6);
    CHECK(error_column("{1,}") == 4);                        // trailing comma
    CHECK(error_column("{,}") == 2);
    CHECK(error_column("[]") == 2);
    CHECK(error_column("()") == 2);
    CHECK_THROWS_AS(P("[1, 2) [3, 4)"), parse::ParseError);  // two sets side by side
}

TEST_CASE("parser: word operators need their operands") {
    CHECK(error_column("U A") == 1);                         // 'U' is an operator and needs a left operand
    CHECK(error_column("in A") == 1);
    CHECK(error_column("and") == 1);
    CHECK(error_column("not") == 4);                         // end of input after `not`
    CHECK(error_column("x in") == 5);
    CHECK(error_column("A U") == 4);
    CHECK_THROWS_AS(P("x y"), parse::ParseError);
}

TEST_CASE("parser: a binder can sit inside set expressions") {
    CHECK(text("sum(i, 1, 3, i) in {6}") == "(sum(i, 1, 3, i) in {6})");
    CHECK(text("x in [0, sum(i, 1, 3, i))") == "(x in [0, sum(i, 1, 3, i)))");
}

// --------------------------------------------------------------- evaluation

TEST_CASE("evaluate: relations on reals") {
    CHECK(truth("1 < 2"));
    CHECK_FALSE(truth("2 < 2"));
    CHECK(truth("2 <= 2"));
    CHECK(truth("3 > 2"));
    CHECK_FALSE(truth("2 > 2"));
    CHECK(truth("3 >= 3"));
    CHECK(truth("2 = 2"));
    CHECK_FALSE(truth("2 = 3"));
    CHECK(truth("2 != 3"));
    CHECK(truth("1 + 1 = 2"));
    CHECK(truth("2 ^ 10 > 1000"));
    CHECK(truth("sin(0) = 0"));
    CHECK_FALSE(truth("0.1 + 0.2 = 0.3"));                   // exact floating-point comparison, not "close enough"
    CHECK(truth("-inf < 0"));
    CHECK(truth("1e308 * 10 = inf"));                        // overflow gives infinity, as in IEEE arithmetic
}

TEST_CASE("evaluate: logic, with no short-circuiting") {
    CHECK(truth("true and true"));
    CHECK_FALSE(truth("true and false"));
    CHECK(truth("false or true"));
    CHECK_FALSE(truth("false or false"));
    CHECK(truth("not false"));
    CHECK_FALSE(truth("not true"));
    CHECK(truth("1 < 2 and 2 < 3"));
    CHECK(truth("1 > 2 or 2 < 3"));
    CHECK(truth("not (1 > 2) and not (3 < 2)"));
    // De Morgan on all four assignments
    for (bool p : {false, true})
        for (bool q : {false, true}) {
            Environment<Value> env;
            env.bind("p", Value::boolean(p));
            env.bind("q", Value::boolean(q));
            auto lhs = parse::Parser().parse("not (p and q)", BOOLS);
            auto rhs = parse::Parser().parse("not p or not q", BOOLS);
            CHECK(evaluate_value(*lhs, env).as_bool() == evaluate_value(*rhs, env).as_bool());
        }
    // `and` evaluates both sides, so the division by zero on the right is reached
    CHECK_THROWS_AS(truth("false and 1 / 0 < 1"), std::invalid_argument);
    CHECK_THROWS_AS(truth("true or 1 / 0 < 1"), std::invalid_argument);
}

TEST_CASE("evaluate: membership") {
    Environment<Value> env;
    auto in_set = [&](const char* src, double x) {
        env.bind("x", Value::real(x));
        return evaluate_value(*P(src), env).as_bool();
    };
    CHECK(in_set("x in [0, 1)", 0));
    CHECK(in_set("x in [0, 1)", 0.5));
    CHECK_FALSE(in_set("x in [0, 1)", 1));
    CHECK_FALSE(in_set("x in (0, 1]", 0));
    CHECK(in_set("x in (0, 1]", 1));
    CHECK(in_set("x in [0, 1) U {5}", 5));
    CHECK_FALSE(in_set("x in [0, 1) U {5}", 3));
    CHECK(in_set("x in {1, 2, 3}", 3));
    CHECK_FALSE(in_set("x in {1, 2, 3}", 4));
    CHECK_FALSE(in_set("x in ∅", 1));
    CHECK(in_set("x in ℝ", 1e9));
    CHECK(in_set("x in complement([0, 1])", 2));
    CHECK_FALSE(in_set("x in complement([0, 1])", 0.5));
    CHECK(in_set("x in (-inf, 0] U (5, inf)", -100));
    CHECK(in_set("x in (-inf, 0] U (5, inf)", 0));
    CHECK_FALSE(in_set("x in (-inf, 0] U (5, inf)", 3));
    CHECK(in_set("x in (-inf, 0] U (5, inf)", 6));
    CHECK(in_set("x * 2 in [0, 4]", 2));                     // the left side is an expression
    CHECK(in_set("x in [x - 1, x + 1]", 7));                 // so are the endpoints
}

TEST_CASE("evaluate: set operations follow the sets library") {
    auto same = [](const char* src, const S::Set& expected) {
        INFO("expression: " << src);
        CHECK(V(src).as_set().equals(expected));
    };
    same("[0, 2] ∩ [1, 3]", *S::IntervalSet::closed(1, 2));
    same("[0, 1] U (1, 2)", *S::IntervalSet::closed_open(0, 2));
    same("{1, 2} U {2, 3}", S::FiniteSet({1, 2, 3}));
    same("{1, 2} ∩ {2, 3}", S::FiniteSet({2}));
    same("complement(∅)", S::UniversalSet::instance());
    same("complement(ℝ)", S::EmptySet::instance());
    same("∅ U [0, 1]", *S::IntervalSet::closed(0, 1));
    same("ℝ ∩ [0, 1]", *S::IntervalSet::closed(0, 1));
    same("[1, 2] ∩ [3, 4]", S::EmptySet::instance());
    same("[0, 1 + 1) ∩ [2 * 0, 5]", *S::IntervalSet::closed_open(0, 2));    // endpoints are evaluated

    // two open intervals that only touch must leave the shared point out
    Value gap = V("(0, 1) U (1, 2)");
    CHECK_FALSE(gap.as_set().contains(1));
    CHECK(gap.as_set().contains(0.5));
    CHECK(gap.as_set().contains(1.5));
}

TEST_CASE("evaluate: equality of sets and of values with the same sort") {
    CHECK(truth("[0, 1] = [0, 1]"));
    CHECK_FALSE(truth("[0, 1] = [0, 1)"));
    CHECK(truth("{1, 2} = {2, 1}"));
    CHECK(truth("∅ = ∅"));
    CHECK_FALSE(truth("ℝ = ∅"));
    CHECK(truth("ℝ != ∅"));
    CHECK(truth("[0, 1] U [2, 3] = [2, 3] U [0, 1]"));
    // structural: the same numbers in a different kind of set are not equal (see Value::equals)
    CHECK_FALSE(truth("{1} = [1, 1]"));
    CHECK(truth("true = true"));
    CHECK(truth("true != false"));
    CHECK(truth("(1 < 2) = true"));
}

TEST_CASE("evaluate: comparing values of different sorts is an error, not false") {
    // It parses (`=` accepts any sorts) but cannot be evaluated.
    CHECK_NOTHROW(P("1 = {1}"));
    CHECK_THROWS_AS(truth("1 = {1}"), std::invalid_argument);
    CHECK_THROWS_WITH(truth("1 = {1}"), doctest::Contains("cannot compare a Real with a Set"));
    CHECK_THROWS_AS(truth("true = 1"), std::invalid_argument);
    CHECK_THROWS_AS(truth("{1} != true"), std::invalid_argument);
}

TEST_CASE("evaluate: variables of every sort come from the environment") {
    Environment<Value> env;
    env.bind("A", Value::set(S::IntervalSet::closed(0, 1)));
    env.bind("B", Value::set(S::IntervalSet::closed(2, 3)));
    env.bind("x", Value::real(2.5));
    CHECK(evaluate_value(*P("x in A U B"), env).as_bool());
    CHECK_FALSE(evaluate_value(*P("x in A"), env).as_bool());
    CHECK(evaluate_value(*P("A ∩ B = ∅"), env).as_bool());
    CHECK(evaluate_value(*P("A U B"), env).as_set().contains(0.5));

    env.bind("p", Value::boolean(true));
    CHECK(evaluate_value(*P("p and x < 3", BOOLS), env).as_bool());
}

TEST_CASE("evaluate: a binding of the wrong sort is caught when it is used") {
    Environment<Value> env;
    env.bind("x", Value::set(S::IntervalSet::closed(0, 1)));   // x is declared nowhere, so the parser made it a Real
    CHECK_THROWS_AS(evaluate_value(*P("x + 1"), env), std::invalid_argument);
    CHECK_THROWS_WITH(evaluate_value(*P("x + 1"), env), doctest::Contains("expected a Real"));

    Environment<Value> env2;
    env2.bind("A", Value::real(1));
    env2.bind("B", Value::real(2));
    CHECK_THROWS_AS(evaluate_value(*P("A U B"), env2), std::invalid_argument);

    CHECK_THROWS_AS(V("x in A"), std::out_of_range);           // unbound
}

TEST_CASE("evaluate: arithmetic over Values agrees with the real interpretation") {
    for (const char* src : {"x * x + 3 * x + 1", "sin(x) * cos(x) - exp(x / 3)", "sum(i, 1, 5, i * x)", "sqrt(x) ^ 2 - 1 / x"}) {
        for (double x : {0.5, 1.0, 2.5, 7.0}) {
            INFO("expression: " << src << "  x = " << x);
            Environment<double> renv;
            renv.bind("x", x);
            Environment<Value> venv;
            venv.bind("x", Value::real(x));
            auto e = parse::Parser().parse(src);
            CHECK(evaluate_value(*e, venv).as_real() == doctest::Approx(evaluate_real(*e, renv)));
        }
    }
    CHECK(V("sum(i, 1, 4, i) in {10}").as_bool());
    CHECK(truth("sum(i, 1, 3, i) = 6"));
    CHECK(truth("prod(i, 1, 5, i) > sum(i, 1, 5, i)"));
}

TEST_CASE("evaluate: errors keep their types") {
    CHECK_THROWS_AS(V("1 / 0 < 1"), std::invalid_argument);
    CHECK_THROWS_AS(V("sum(i, 0.5, 3, i) in {1}"), std::invalid_argument);
    CHECK_THROWS_AS(evaluate_real(*parse::Parser().parse("1 < 2")), std::out_of_range);   // reals alone can't do relations
    CHECK(evaluate_value(*parse::Parser().parse("1 + 2")).as_real() == 3.0);
}

// --------------------------------------------------------------- the sets library's own text

TEST_CASE("the sets library's text parses and evaluates to an equal set") {
    std::vector<std::unique_ptr<S::Set>> corpus;
    corpus.push_back(S::IntervalSet::closed(0, 1));
    corpus.push_back(S::IntervalSet::open(0, 1));
    corpus.push_back(S::IntervalSet::closed_open(-2.5, 7));
    corpus.push_back(S::IntervalSet::open_closed(0.1, 0.2));
    corpus.push_back(S::IntervalSet::non_negative_real());
    corpus.push_back(S::IntervalSet::negative_real());
    corpus.push_back(S::IntervalSet::all_real());
    corpus.push_back(std::make_unique<S::FiniteSet>(std::initializer_list<double>{1, 2, 3}));
    corpus.push_back(std::make_unique<S::FiniteSet>(std::initializer_list<double>{}));
    corpus.push_back(std::make_unique<S::FiniteSet>(std::initializer_list<double>{0.1, 1.0 / 3.0, -4}));
    corpus.push_back(S::EmptySet::instance().clone());
    corpus.push_back(S::UniversalSet::instance().clone());
    for (const auto& set : corpus) {
        INFO("set text: " << set->toString());
        Value v = V(set->toString());
        CHECK(v.as_set().equals(*set));
    }
}

TEST_CASE("the sets library's text for composite sets keeps its meaning") {
    auto closed01 = S::IntervalSet::closed(0, 1);
    auto five = S::FiniteSet({5});
    auto u = closed01->set_union(five);                                    // ([0, 1] U {5})
    CHECK(u->toString() == "([0, 1] U {5})");
    CHECK(V(u->toString()).as_set().equals(*u));                           // evaluates to an equal UnionSet
    check_same_membership(*u, u->toString());
    check_same_membership(*u->complement(), u->complement()->toString());  // nested unions and intersections
    check_same_membership(*u->intersect(*S::IntervalSet::closed(0.5, 6)), u->intersect(*S::IntervalSet::closed(0.5, 6))->toString());

    auto fin = S::FiniteSet({1, 2, 3});
    check_same_membership(*fin.complement(), fin.complement()->toString());
    auto two = S::IntervalSet::closed(3, 4)->set_union(*S::IntervalSet::closed(0, 1));
    CHECK(two->toString() == "([3, 4] U [0, 1])");
    check_same_membership(*two->complement(), two->complement()->toString());
}

TEST_CASE("core prints a set the way the sets library does (for finite endpoints)") {
    for (const char* src : {"[0, 1)", "(0, 1]", "{1, 2}", "{}", "∅", "ℝ", "([0, 1) U {1, 2})", "((0, 1) ∩ {1, 2})", "[0.1, 0.2]"})
        CHECK(text(src) == std::string(src));
    // for infinite endpoints core writes the negative sign as an operator, so (-inf) has parentheses
    CHECK(text("(-inf, 0]") == "((-inf), 0]");
    CHECK(V("(-inf, 0]").as_set().equals(*S::IntervalSet::non_positive_real()));
}

// --------------------------------------------------------------- round trips and the existing tools

TEST_CASE("round trip: parse(print(e)) equals e for set expressions") {
    std::vector<ExprPtr> corpus;
    corpus.push_back(interval(num(0), num(1), true, false));
    corpus.push_back(interval(neg(infinity()), num(0), false, true));
    corpus.push_back(interval(add(var("a"), num(1)), mul(num(2), var("b")), false, false));
    corpus.push_back(finite_set({}));
    corpus.push_back(empty_set());
    corpus.push_back(reals());
    corpus.push_back(set_union(setvar("A"), intersection(setvar("B"), setvar("C"))));
    corpus.push_back(intersection(set_union(setvar("A"), setvar("B")), complement(setvar("C"))));
    corpus.push_back(complement(set_union(empty_set(), reals())));
    corpus.push_back(in(var("x"), set_union(setvar("A"), interval(num(0), num(1), true, true))));
    corpus.push_back(in(sum("i", num(1), num(3), var("i")), finite_set({})));
    corpus.push_back(eq(setvar("A"), setvar("B")));
    corpus.push_back(ne(boolean(true), logical_not(boolean(false))));
    corpus.push_back(lt(add(var("x"), num(1)), mul(var("y"), num(2))));
    corpus.push_back(logical_or(logical_and(le(var("x"), num(3)), ge(var("y"), num(2))), logical_not(in(var("x"), setvar("A")))));
    corpus.push_back(logical_not(logical_not(logical_and(boolean(true), boolean(false)))));
    corpus.push_back(ne(var("x"), infinity()));
    for (const auto& e : corpus) {
        INFO("expression: " << e->toString());
        auto back = P(e->toString());
        CHECK(back->equals(*e));
        CHECK(back->toString() == e->toString());
    }
}

TEST_CASE("round trip: boolean variables") {
    auto e = logical_or(logical_and(boolvar("p"), boolvar("q")), logical_not(boolvar("p")));
    auto back = parse::Parser().parse(e->toString(), BOOLS);
    CHECK(back->equals(*e));
}

TEST_CASE("analysis, substitution and simplification work on set expressions") {
    auto e = P("x in A U B and y < 3");
    CHECK(free_variables(*e) == std::set<std::string>{"A", "B", "x", "y"});
    CHECK(node_count(*P("x in A")) == 3);

    Substitution s;
    s.bind("x", *num(2));
    s.bind("A", *P("[0, 5)"));
    CHECK(s.apply(*P("x in A"))->toString() == "(2 in [0, 5))");

    Substitution wrong;
    wrong.bind("A", *num(1));                       // a Real where a Set variable is
    CHECK_THROWS_AS(wrong.apply(*P("x in A")), std::invalid_argument);

    // simplify tidies the Real parts and leaves the set structure alone
    CHECK(simplify(*P("x in [0 + 0, 1 * 1) U {2 + 3}"))->toString() == "(x in ([0, 1) U {5}))");
    CHECK(simplify(*P("x + 0 < 1 * 3 and true"))->toString() == "((x < 3) and true)");
    auto already = P("x in A U B");
    CHECK(simplify(*already)->equals(*already));
}

TEST_CASE("binders with sets in their bodies and bounds") {
    // a bound variable is a Real, so it can be tested for membership
    CHECK(free_variables(*P("sum(i, 1, n, i)")) == std::set<std::string>{"n"});
    Environment<Value> env;
    env.bind("n", Value::real(4));
    CHECK(evaluate_value(*P("sum(i, 1, n, i) in [10, 10]"), env).as_bool());
}

TEST_CASE("differentiation has no rule for the new operators, and says so") {
    CHECK_THROWS_AS(cppmath::calculus::differentiate(*P("x in A"), "x"), std::invalid_argument);
    CHECK_THROWS_WITH(cppmath::calculus::differentiate(*P("x < 3"), "x"), doctest::Contains("'lt'"));
    CHECK(cppmath::calculus::differentiate(*P("x * inf"), "x")->toString() == "inf");   // `inf` is a constant, so d(inf) = 0
}
