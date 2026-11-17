#include "cppmath/core/RealInterpretation.hpp"
#include <cmath>
#include <stdexcept>

namespace cppmath {
namespace core {

namespace {
using Args = std::vector<double>;

double checked(double result, const char* what) {
    if (std::isnan(result)) throw std::invalid_argument(std::string(what) + ": result is undefined");
    return result;
}

// Reads the integer bounds [lo, hi] of a sum/prod and guards against loops that would never finish.
constexpr long long MAX_TERMS = 10000000;

struct IntRange { long long lo, hi; };

IntRange integer_range(const std::vector<double>& bounds, const char* what) {
    for (double v : bounds)
        // NaN fails the first test, +-infinity and huge values the second.
        if (v != std::floor(v) || std::fabs(v) > 1e15)
            throw std::invalid_argument(std::string(what) + ": bounds must be finite integers");
    IntRange r{static_cast<long long>(bounds[0]), static_cast<long long>(bounds[1])};
    if (r.hi >= r.lo && r.hi - r.lo >= MAX_TERMS)
        throw std::invalid_argument(std::string(what) + ": more than " + std::to_string(MAX_TERMS) + " terms");
    return r;
}

Interpretation<double> build_real() {
    Interpretation<double> r([](double v) { return v; });
    r.define("add", [](const Args& a) { return a[0] + a[1]; });
    r.define("sub", [](const Args& a) { return a[0] - a[1]; });
    r.define("mul", [](const Args& a) { return a[0] * a[1]; });
    r.define("div", [](const Args& a) {
        if (a[1] == 0.0) throw std::invalid_argument("div: division by zero");
        return a[0] / a[1];
    });
    r.define("pow", [](const Args& a) { return checked(std::pow(a[0], a[1]), "pow"); });
    r.define("neg", [](const Args& a) { return -a[0]; });
    r.define("sin", [](const Args& a) { return std::sin(a[0]); });
    r.define("cos", [](const Args& a) { return std::cos(a[0]); });
    r.define("tan", [](const Args& a) { return std::tan(a[0]); });
    r.define("exp", [](const Args& a) { return std::exp(a[0]); });
    r.define("log", [](const Args& a) {
        if (a[0] <= 0.0) throw std::invalid_argument("log: argument must be positive");
        return std::log(a[0]);
    });
    r.define("sqrt", [](const Args& a) {
        if (a[0] < 0.0) throw std::invalid_argument("sqrt: argument must be non-negative");
        return std::sqrt(a[0]);
    });
    r.define("abs", [](const Args& a) { return std::fabs(a[0]); });
    // sum(i, lo, hi, body) = body(lo) + ... + body(hi), 0 if hi < lo; prod is the same with * and 1.
    r.define_binder("sum", [](const Args& bounds, const std::function<double(const double&)>& body) {
        IntRange range = integer_range(bounds, "sum");
        double acc = 0.0;
        for (long long k = range.lo; k <= range.hi; ++k) acc += body(static_cast<double>(k));
        return acc;
    });
    r.define_binder("prod", [](const Args& bounds, const std::function<double(const double&)>& body) {
        IntRange range = integer_range(bounds, "prod");
        double acc = 1.0;
        for (long long k = range.lo; k <= range.hi; ++k) acc *= body(static_cast<double>(k));
        return acc;
    });
    r.define("pi", [](const Args&) { return 3.14159265358979323846; });
    r.define("e", [](const Args&) { return 2.71828182845904523536; });
    return r;
}
} // namespace

const Interpretation<double>& real_interpretation() {
    static const Interpretation<double> interp = build_real();
    return interp;
}

double evaluate_real(const Expression& expr, const Environment<double>& env) {
    return evaluate<double>(expr, real_interpretation(), env);
}

} // namespace core
} // namespace cppmath
