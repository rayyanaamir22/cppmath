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
