// core_demo: parse one expression and evaluate it in two different algebras.
#include <iomanip>
#include <iostream>
#include <memory>
#include "cppmath/abstract_algebra/field/FieldInterpretation.hpp"
#include "cppmath/calculus/diff.hpp"
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/Simplify.hpp"
#include "cppmath/core/Substitution.hpp"
#include "cppmath/core/parse/ParseError.hpp"
#include "cppmath/core/parse/Parser.hpp"

using namespace cppmath::core;
using cppmath::abstract_algebra::example::FiniteField;
using cppmath::abstract_algebra::example::FiniteFieldElement;
using cppmath::abstract_algebra::example::finite_field_interpretation;

int main() {
    // Text -> tree. The printed form is canonical and fully parenthesised.
    auto expr = parse::Parser().parse("x*x + 3*x + 1");
    std::cout << "expression : " << expr->toString() << "\n";
    std::cout << "variables  : ";
    for (const auto& v : free_variables(*expr)) std::cout << v << " ";
    std::cout << "(" << node_count(*expr) << " nodes, depth " << tree_depth(*expr) << ")\n\n";

    // Same tree, two meanings: reals, and the integers mod 7.
    auto f7 = std::make_shared<FiniteField>(7);
    auto in_f7 = finite_field_interpretation(f7);
    std::cout << " x | over R | in F_7\n---+--------+------\n";
    for (int x = 0; x < 7; ++x) {
        Environment<double> real_env;
        real_env.bind("x", x);
        Environment<FiniteFieldElement> f7_env;
        f7_env.bind("x", FiniteFieldElement(f7, x));
        std::cout << std::setw(2) << x << " | " << std::setw(6) << evaluate_real(*expr, real_env) << " | "
                  << std::setw(4) << evaluate(*expr, in_f7, f7_env).int_value() << "\n";
    }

    // Division only makes sense where the algebra allows it.
    auto inverse = parse::Parser().parse("1 / x");
    Environment<FiniteFieldElement> env;
    env.bind("x", FiniteFieldElement(f7, 3));
    std::cout << "\n1/3 in F_7 = " << evaluate(*inverse, in_f7, env).int_value() << " (since 3*5 = 15 = 1 mod 7)\n";
    env.bind("x", FiniteFieldElement(f7, 0));
    try {
        evaluate(*inverse, in_f7, env);
    } catch (const std::invalid_argument& e) {
        std::cout << "1/0 in F_7 -> " << e.what() << "\n";
    }

    // An operator the algebra doesn't define is reported, not guessed.
    try {
        evaluate(*parse::Parser().parse("sin(x)"), in_f7, env);
    } catch (const std::out_of_range& e) {
        std::cout << "sin(x) in F_7 -> " << e.what() << "\n";
    }

    // Binders: sums and products with a bound variable. The bound i is local to the body.
    auto series = parse::Parser().parse("sum(i, 1, n, i * i)");
    Environment<double> n_env;
    n_env.bind("n", 10);
    std::cout << "\n" << series->toString() << " with n = 10 is " << evaluate_real(*series, n_env)
              << "  (free variables: ";
    for (const auto& v : free_variables(*series)) std::cout << v << " ";
    std::cout << ")\n";

    // Substitution is syntactic: it works on the tree, before any algebra is chosen.
    Substitution s;
    s.bind("x", *parse::Parser().parse("y + 1"));
    std::cout << "\nx := y + 1  gives  " << s.apply(*expr)->toString() << "\n";

    // ... and it never lets a binder capture a variable: renaming happens only when needed.
    auto scaled = parse::Parser().parse("sum(i, 1, 3, i * n)");
    Substitution capture;
    capture.bind("n", *parse::Parser().parse("i"));
    std::cout << scaled->toString() << "\n  with n := i  gives  " << capture.apply(*scaled)->toString()
              << "   (the binder's i was renamed so the new i stays free)\n";

    // The tree can be differentiated and tidied symbolically, then evaluated like any other.
    for (const char* src : {"x*x + 3*x + 1", "sin(x*x)", "x^x", "exp(2*x) / x"}) {
        auto f = parse::Parser().parse(src);
        auto df = cppmath::calculus::differentiate(*f, "x");
        Environment<double> at;
        at.bind("x", 1.5);
        std::cout << "d/dx " << f->toString() << "  =  " << df->toString() << "   (at x=1.5: " << evaluate_real(*df, at) << ")\n";
    }
    std::cout << "simplify((x + 0) * 1 + 2 * 3) = " << simplify(*parse::Parser().parse("(x + 0) * 1 + 2 * 3"))->toString() << "\n";

    // Errors in the text carry positions.
    try {
        parse::Parser().parse("2 * (x + ");
    } catch (const parse::ParseError& e) {
        std::cout << "parse error: " << e.what() << "\n";
    }
    return 0;
}
