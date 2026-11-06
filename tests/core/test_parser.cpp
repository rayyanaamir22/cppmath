#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cmath>
#include <string>
#include <vector>
#include "cppmath/core/Builders.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/parse/Lexer.hpp"
#include "cppmath/core/parse/ParseError.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::core;
using namespace cppmath::core::build;
using namespace cppmath::core::parse;

namespace {
std::string canon(const std::string& s) { return Parser().parse(s)->toString(); }
std::vector<TokenKind> kinds(const std::string& s) {
    std::vector<TokenKind> out;
    for (auto& t : Lexer().tokenize(s)) out.push_back(t.kind);
    return out;
}
}

// ---------------------------------------------------------------- lexer

TEST_CASE("lexer: token kinds and text") {
    auto toks = Lexer().tokenize("sin(x1) + 2.5");
    REQUIRE(toks.size() == 7);
    CHECK(toks[0].kind == TokenKind::Identifier);
    CHECK(toks[0].text == "sin");
    CHECK(toks[1].kind == TokenKind::LParen);
    CHECK(toks[2].text == "x1");
    CHECK(toks[3].kind == TokenKind::RParen);
    CHECK(toks[4].kind == TokenKind::Operator);
    CHECK(toks[5].kind == TokenKind::Number);
    CHECK(toks[5].number == doctest::Approx(2.5));
    CHECK(toks[6].kind == TokenKind::End);
}

TEST_CASE("lexer: numbers") {
    CHECK(Lexer().tokenize("1e-3")[0].number == doctest::Approx(0.001));
    CHECK(Lexer().tokenize("1E+2")[0].number == doctest::Approx(100.0));
    CHECK(Lexer().tokenize("12.")[0].number == doctest::Approx(12.0));
    // 'e' with no digits after it is not an exponent: number 2, then identifier e
    CHECK(kinds("2e") == std::vector<TokenKind>{TokenKind::Number, TokenKind::Identifier, TokenKind::End});
    CHECK(kinds("2e+") == std::vector<TokenKind>{TokenKind::Number, TokenKind::Identifier, TokenKind::Operator, TokenKind::End});
}

TEST_CASE("lexer: positions across lines") {
    auto toks = Lexer().tokenize("x +\n  yy");
    CHECK(toks[0].span.line == 1);
    CHECK(toks[0].span.column == 1);
    CHECK(toks[1].span.column == 3);
    CHECK(toks[2].text == "yy");
    CHECK(toks[2].span.line == 2);
    CHECK(toks[2].span.column == 3);
    CHECK(toks[2].span.offset == 6);
    CHECK(toks[2].span.length == 2);
}

TEST_CASE("lexer: unknown character is a ParseError with position") {
    try {
        Lexer().tokenize("1 + $");
        FAIL("expected ParseError");
    } catch (const ParseError& e) {
        CHECK(e.span().column == 5);
        CHECK(std::string(e.what()).find("1:5") == 0);
    }
}

TEST_CASE("lexer: longest operator symbol wins") {
    const Sort R = Sort::Real();
    Signature sig;
    sig.add(Operator("lt", "<", Arity::fixed(2), {R, R}, Sort::Bool(), Notation::Infix, 5));
    sig.add(Operator("le", "<=", Arity::fixed(2), {R, R}, Sort::Bool(), Notation::Infix, 5));
    sig.add(Operator("shl", "<<", Arity::fixed(2), {R, R}, R, Notation::Infix, 5));
    auto toks = Lexer(sig).tokenize("a<=b<<c<d");
    std::vector<std::string> ops;
    for (auto& t : toks) if (t.kind == TokenKind::Operator) ops.push_back(t.text);
    CHECK(ops == std::vector<std::string>{"<=", "<<", "<"});
}

// --------------------------------------------------------------- parser

TEST_CASE("parser: atoms") {
    CHECK(Parser().parse("42")->equals(*num(42)));
    CHECK(Parser().parse("x")->equals(*var("x")));
    CHECK(Parser().parse("pi")->equals(*pi()));
    CHECK(Parser().parse("e")->equals(*euler()));
    CHECK(Parser().parse("  ( x )  ")->equals(*var("x")));
}

TEST_CASE("parser: precedence") {
    CHECK(canon("1 + 2 * 3") == "(1 + (2 * 3))");
    CHECK(canon("1 * 2 + 3") == "((1 * 2) + 3)");
    CHECK(canon("(1 + 2) * 3") == "((1 + 2) * 3)");
    CHECK(canon("2 * 3 ^ 2") == "(2 * (3 ^ 2))");
    CHECK(canon("a / b * c") == "((a / b) * c)");
}

TEST_CASE("parser: associativity") {
    CHECK(canon("1 - 2 - 3") == "((1 - 2) - 3)");          // left
    CHECK(canon("8 / 4 / 2") == "((8 / 4) / 2)");          // left
    CHECK(canon("2 ^ 3 ^ 2") == "(2 ^ (3 ^ 2))");          // right
    CHECK(evaluate_real(*Parser().parse("2 ^ 3 ^ 2")) == doctest::Approx(512.0));
    CHECK(evaluate_real(*Parser().parse("10 - 4 - 3")) == doctest::Approx(3.0));
}

TEST_CASE("parser: prefix minus") {
    CHECK(canon("-x") == "(-x)");
    CHECK(canon("-x ^ 2") == "(-(x ^ 2))");                // ^ binds tighter than prefix -
    CHECK(canon("-x * y") == "((-x) * y)");                // prefix - binds tighter than *
    CHECK(canon("a - -b") == "(a - (-b))");
    CHECK(canon("2 ^ -3") == "(2 ^ (-3))");
    CHECK(canon("--x") == "(-(-x))");
    CHECK(evaluate_real(*Parser().parse("-2 ^ 2")) == doctest::Approx(-4.0));
}

TEST_CASE("parser: function calls") {
    CHECK(canon("sin(x)") == "sin(x)");
    CHECK(canon("sin(x + 1) * 2") == "(sin((x + 1)) * 2)");
    CHECK(canon("exp(log(x))") == "exp(log(x))");
    CHECK(canon("sin ( x )") == "sin(x)");
}

TEST_CASE("parser: variadic function with a custom signature") {
    const Sort R = Sort::Real();
    Signature sig;
    sig.extend(Signature::standard());
    sig.add(Operator("max", "max", Arity::at_least(2), {R}, R, Notation::FunctionCall));
    Parser p(sig);
    CHECK(p.parse("max(1, x, 3 + 4)")->toString() == "max(1, x, (3 + 4))");
    CHECK_THROWS_AS(p.parse("max(1)"), ParseError);   // arity
}

TEST_CASE("parser: errors carry positions") {
    auto column_of_error = [](const std::string& src) -> std::size_t {
        try { Parser().parse(src); } catch (const ParseError& e) { return e.span().column; }
        return 0;
    };
    CHECK(column_of_error("1 +") == 4);        // unexpected end
    CHECK(column_of_error("(1 + 2") == 7);     // missing ')'
    CHECK(column_of_error("1 2") == 3);        // trailing token
    CHECK(column_of_error("* 3") == 1);        // no prefix '*'
    CHECK(column_of_error("foo(1)") == 1);     // unknown function
    CHECK(column_of_error("sin") == 1);        // function name without '('
    CHECK(column_of_error("1 + )") == 5);
    CHECK(column_of_error("") == 1);
    CHECK(column_of_error("sin(1, 2)") == 1);  // arity, reported at the function
}

TEST_CASE("parser: ParseError is an invalid_argument") {
    CHECK_THROWS_AS(Parser().parse("1 +"), std::invalid_argument);
    CHECK_THROWS_WITH(Parser().parse("1 +"), doctest::Contains("1:4"));
}

TEST_CASE("parser: no implicit multiplication") {
    CHECK_THROWS_AS(Parser().parse("2x"), ParseError);
    CHECK_THROWS_AS(Parser().parse("x y"), ParseError);
}

// ------------------------------------------------------------ round trip

TEST_CASE("round trip: parse(print(e)) equals e for built trees") {
    std::vector<ExprPtr> corpus;
    corpus.push_back(num(0));
    corpus.push_back(num(0.1));
    corpus.push_back(num(1.0 / 3.0));
    corpus.push_back(num(1e20));
    corpus.push_back(num(1e-20));
    corpus.push_back(num(123456789.123456789));
    corpus.push_back(num(-3));
    corpus.push_back(var("x"));
    corpus.push_back(pi());
    corpus.push_back(add(var("x"), num(1)));
    corpus.push_back(sub(sub(num(1), num(2)), num(3)));
    corpus.push_back(sub(num(1), sub(num(2), num(3))));
    corpus.push_back(pow(pow(num(2), num(3)), num(2)));
    corpus.push_back(pow(num(2), pow(num(3), num(2))));
    corpus.push_back(neg(pow(var("x"), num(2))));
    corpus.push_back(pow(neg(var("x")), num(2)));
    corpus.push_back(neg(neg(var("x"))));
    corpus.push_back(mul(neg(var("a")), div(var("b"), num(-2))));
    corpus.push_back(sin(add(cos(var("x")), exp(mul(pi(), var("y"))))));
    corpus.push_back(div(sqrt(abs(log(var("t")))), tan(euler())));
    corpus.push_back(add(mul(add(var("x"), num(2)), sin(var("y"))), pow(var("z"), neg(num(3)))));

    for (const auto& e : corpus) {
        INFO("expression: " << e->toString());
        auto reparsed = Parser().parse(e->toString());
        CHECK(reparsed->equals(*e));
        CHECK(reparsed->toString() == e->toString());   // print is a fixed point
    }
}

TEST_CASE("round trip: print(parse(s)) is canonical, and parse is stable") {
    for (std::string s : {"1+2*3", "-x^2", "sin(x)^2 + cos(x)^2", "2^3^2", "a-b-c", "((x))", "1e3 * x"}) {
        INFO("source: " << s);
        std::string once = canon(s);
        CHECK(canon(once) == once);
    }
}

TEST_CASE("parsed trees evaluate") {
    Environment<double> env;
    env.bind("x", 2.0);
    CHECK(evaluate_real(*Parser().parse("x^2 + 3*x + 1"), env) == doctest::Approx(11.0));
    CHECK(evaluate_real(*Parser().parse("sin(pi / 2) + cos(0)")) == doctest::Approx(2.0));
    CHECK(evaluate_real(*Parser().parse("1e3 / 4")) == doctest::Approx(250.0));
}
