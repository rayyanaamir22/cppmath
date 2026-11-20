#include "cppmath/core/ValueInterpretation.hpp"
#include "cppmath/core/RealInterpretation.hpp"
#include "cppmath/core/Signature.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/FiniteSet.hpp"
#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

namespace cppmath {
namespace core {

namespace {
using Args = std::vector<Value>;

// Give `name` the meaning it has over the reals, applied to Real values.
void lift_real(Interpretation<Value>& out, const char* name) {
    const Operator& op = Signature::standard().get(name);   // only op.name() is used by apply
    out.define(name, [&op](const Args& args) {
        std::vector<double> reals;
        reals.reserve(args.size());
        for (const Value& a : args) reals.push_back(a.as_real());
        return Value::real(real_interpretation().apply(op, reals));
    });
}

void lift_real_binder(Interpretation<Value>& out, const char* name) {
    const Operator& op = Signature::standard().get(name);
    out.define_binder(name, [&op](const Args& bounds, const std::function<Value(const Value&)>& body) {
        std::vector<double> reals;
        for (const Value& b : bounds) reals.push_back(b.as_real());
        auto real_body = [&body](const double& i) { return body(Value::real(i)).as_real(); };
        return Value::real(real_interpretation().apply_binder(op, reals, real_body));
    });
}

bool same_sort_equal(const Value& a, const Value& b) {
    if (a.sort() != b.sort())
        throw std::invalid_argument("cannot compare a " + a.sort().name() + " with a " + b.sort().name());
    return a.equals(b);
}

template <class Compare>
void define_relation(Interpretation<Value>& out, const char* name, Compare cmp) {
    out.define(name, [cmp](const Args& a) { return Value::boolean(cmp(a[0].as_real(), a[1].as_real())); });
}

void define_interval(Interpretation<Value>& out, const char* name, bool lo_closed, bool hi_closed) {
    out.define(name, [lo_closed, hi_closed](const Args& a) {
        return Value::set(std::make_unique<sets::IntervalSet>(a[0].as_real(), a[1].as_real(), lo_closed, hi_closed));
    });
}

Interpretation<Value> build_value() {
    Interpretation<Value> v([](double x) { return Value::real(x); });

    for (const char* name : {"add", "sub", "mul", "div", "pow", "neg", "sin", "cos", "tan", "exp", "log", "sqrt",
                             "abs", "pi", "e"})
        lift_real(v, name);
    lift_real_binder(v, "sum");
    lift_real_binder(v, "prod");
    v.define("inf", [](const Args&) { return Value::real(std::numeric_limits<double>::infinity()); });

    define_relation(v, "lt", std::less<double>());
    define_relation(v, "le", std::less_equal<double>());
    define_relation(v, "gt", std::greater<double>());
    define_relation(v, "ge", std::greater_equal<double>());
    v.define("eq", [](const Args& a) { return Value::boolean(same_sort_equal(a[0], a[1])); });
    v.define("ne", [](const Args& a) { return Value::boolean(!same_sort_equal(a[0], a[1])); });

    v.define("and", [](const Args& a) { return Value::boolean(a[0].as_bool() && a[1].as_bool()); });
    v.define("or", [](const Args& a) { return Value::boolean(a[0].as_bool() || a[1].as_bool()); });
    v.define("not", [](const Args& a) { return Value::boolean(!a[0].as_bool()); });
    v.define("true", [](const Args&) { return Value::boolean(true); });
    v.define("false", [](const Args&) { return Value::boolean(false); });

    v.define("in", [](const Args& a) { return Value::boolean(a[1].as_set().contains(a[0].as_real())); });
    v.define("union", [](const Args& a) { return Value::set(a[0].as_set().set_union(a[1].as_set())); });
    v.define("intersection", [](const Args& a) { return Value::set(a[0].as_set().intersect(a[1].as_set())); });
    v.define("complement", [](const Args& a) { return Value::set(a[0].as_set().complement()); });
    v.define("empty_set", [](const Args&) { return Value::set(sets::EmptySet::instance().clone()); });
    v.define("universe", [](const Args&) { return Value::set(sets::UniversalSet::instance().clone()); });

    define_interval(v, "interval_cc", true, true);
    define_interval(v, "interval_co", true, false);
    define_interval(v, "interval_oc", false, true);
    define_interval(v, "interval_oo", false, false);
    v.define("finite_set", [](const Args& a) {
        std::vector<double> elements;
        for (const Value& x : a) elements.push_back(x.as_real());
        return Value::set(std::make_unique<sets::FiniteSet>(elements));
    });
    return v;
}
} // namespace

const Interpretation<Value>& value_interpretation() {
    static const Interpretation<Value> interp = build_value();
    return interp;
}

Value evaluate_value(const Expression& expr, const Environment<Value>& env) {
    return evaluate<Value>(expr, value_interpretation(), env);
}

} // namespace core
} // namespace cppmath
