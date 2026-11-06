#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include "cppmath/utils/format.hpp"
#include <algorithm>
#include <sstream>
#include <limits>
#include <cmath>

namespace cppmath {
namespace sets {

namespace {
    // margin of error for double equality
    constexpr double EPSILON = 1e-9;
    // `a == b` first, because inf - inf is NaN and would make equal infinite bounds compare unequal.
    bool double_eq(double a, double b) {
        return a == b || std::fabs(a - b) < EPSILON;
    }

    // True if the union of a and b is one connected interval: neither lies strictly
    // before the other. Where they meet at a single point, at least one must contain it:
    // (0, 1) and (1, 2) leave out 1, so they stay separate; [0, 1] and (1, 2) join.
    bool forms_one_interval(const IntervalSet& a, const IntervalSet& b) {
        auto reaches = [](const IntervalSet& left, const IntervalSet& right) {
            if (double_eq(left.upper_bound, right.lower_bound))
                return left.upper_inclusive || right.lower_inclusive;
            return left.upper_bound > right.lower_bound;
        };
        return reaches(a, b) && reaches(b, a);
    }
}

IntervalSet::IntervalSet(double lower, double upper, bool lower_inc, bool upper_inc)
    : lower_bound(lower), upper_bound(upper), lower_inclusive(lower_inc), upper_inclusive(upper_inc) {}

std::unique_ptr<IntervalSet> IntervalSet::open(double lower, double upper) {
    return std::make_unique<IntervalSet>(lower, upper, false, false);
}
std::unique_ptr<IntervalSet> IntervalSet::closed(double lower, double upper) {
    return std::make_unique<IntervalSet>(lower, upper, true, true);
}
std::unique_ptr<IntervalSet> IntervalSet::open_closed(double lower, double upper) {
    return std::make_unique<IntervalSet>(lower, upper, false, true);
}
std::unique_ptr<IntervalSet> IntervalSet::closed_open(double lower, double upper) {
    return std::make_unique<IntervalSet>(lower, upper, true, false);
}
std::unique_ptr<IntervalSet> IntervalSet::positive_real() {
    return std::make_unique<IntervalSet>(0, std::numeric_limits<double>::infinity(), false, false);
}
std::unique_ptr<IntervalSet> IntervalSet::negative_real() {
    return std::make_unique<IntervalSet>(-std::numeric_limits<double>::infinity(), 0, false, false);
}
std::unique_ptr<IntervalSet> IntervalSet::non_negative_real() {
    return std::make_unique<IntervalSet>(0, std::numeric_limits<double>::infinity(), true, false);
}
std::unique_ptr<IntervalSet> IntervalSet::non_positive_real() {
    return std::make_unique<IntervalSet>(-std::numeric_limits<double>::infinity(), 0, false, true);
}
std::unique_ptr<IntervalSet> IntervalSet::all_real() {
    return std::make_unique<IntervalSet>(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), false, false);
}

bool IntervalSet::contains(double value) const {
    bool left = lower_inclusive ? (value >= lower_bound || double_eq(value, lower_bound)) : (value > lower_bound);
    bool right = upper_inclusive ? (value <= upper_bound || double_eq(value, upper_bound)) : (value < upper_bound);
    return left && right;
}

std::unique_ptr<Set> IntervalSet::set_union(const Set& other) const {
    auto o = dynamic_cast<const IntervalSet*>(&other);
    if (o && forms_one_interval(*this, *o)) {
        double lo = std::min(lower_bound, o->lower_bound);
        double hi = std::max(upper_bound, o->upper_bound);
        // An end is included if *either* interval has that endpoint and includes it.
        bool lo_inc = (double_eq(lower_bound, lo) && lower_inclusive) || (double_eq(o->lower_bound, lo) && o->lower_inclusive);
        bool hi_inc = (double_eq(upper_bound, hi) && upper_inclusive) || (double_eq(o->upper_bound, hi) && o->upper_inclusive);
        return std::make_unique<IntervalSet>(lo, hi, lo_inc, hi_inc);
    }
    // Disjoint, or not an interval: keep both parts.
    return std::make_unique<UnionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> IntervalSet::intersect(const Set& other) const {
    auto o = dynamic_cast<const IntervalSet*>(&other);
    if (o) {
        double lo = std::max(lower_bound, o->lower_bound);
        double hi = std::min(upper_bound, o->upper_bound);
        // An end is included only if *every* interval whose endpoint it is includes it.
        bool lo_inc = (!double_eq(lower_bound, lo) || lower_inclusive) && (!double_eq(o->lower_bound, lo) || o->lower_inclusive);
        bool hi_inc = (!double_eq(upper_bound, hi) || upper_inclusive) && (!double_eq(o->upper_bound, hi) || o->upper_inclusive);
        if (lo < hi || (double_eq(lo, hi) && lo_inc && hi_inc)) {
            return std::make_unique<IntervalSet>(lo, hi, lo_inc, hi_inc);
        }
        return EmptySet::instance().clone();
    }
    return std::make_unique<IntersectionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> IntervalSet::complement() const {
    bool inf_left = std::isinf(lower_bound) && lower_bound < 0;
    bool inf_right = std::isinf(upper_bound) && upper_bound > 0;
    std::unique_ptr<Set> left, right;
    if (!inf_left) {
        left = std::make_unique<IntervalSet>(-std::numeric_limits<double>::infinity(), lower_bound, false, !lower_inclusive);
    }
    if (!inf_right) {
        right = std::make_unique<IntervalSet>(upper_bound, std::numeric_limits<double>::infinity(), !upper_inclusive, false);
    }
    if (left && right) {
        return std::make_unique<UnionSet>(std::move(left), std::move(right));
    } else if (left) {
        return std::move(left);
    } else if (right) {
        return std::move(right);
    } else {
        return EmptySet::instance().clone();
    }
}

std::string IntervalSet::toString() const {
    std::ostringstream oss;
    oss << (lower_inclusive ? "[" : "(");
    if (std::isinf(lower_bound)) oss << (lower_bound < 0 ? "-inf" : "inf");
    else oss << utils::format_number(lower_bound);
    oss << ", ";
    if (std::isinf(upper_bound)) oss << (upper_bound > 0 ? "inf" : "-inf");
    else oss << utils::format_number(upper_bound);
    oss << (upper_inclusive ? "]" : ")");
    return oss.str();
}

std::unique_ptr<Set> IntervalSet::clone() const {
    return std::make_unique<IntervalSet>(lower_bound, upper_bound, lower_inclusive, upper_inclusive);
}

bool IntervalSet::equals(const Set& other) const {
    auto o = dynamic_cast<const IntervalSet*>(&other);
    if (!o) return false;
    return double_eq(lower_bound, o->lower_bound) && double_eq(upper_bound, o->upper_bound)
        && lower_inclusive == o->lower_inclusive && upper_inclusive == o->upper_inclusive;
}

} // namespace sets
} // namespace cppmath 