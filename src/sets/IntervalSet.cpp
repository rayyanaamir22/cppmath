#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include <sstream>
#include <limits>
#include <cmath>

namespace cppmath {
namespace sets {

namespace {
    // margin of error for double equality
    constexpr double EPSILON = 1e-9;
    bool double_eq(double a, double b) {
        return std::fabs(a - b) < EPSILON;
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
    // If other is also IntervalSet, try to merge
    auto other_interval = dynamic_cast<const IntervalSet*>(&other);
    if (other_interval) {
        // Check for overlap or adjacency
        double a1 = lower_bound, b1 = upper_bound;
        double a2 = other_interval->lower_bound, b2 = other_interval->upper_bound;
        bool can_merge = false;
        if (b1 > a2 || double_eq(b1, a2)) can_merge = true;
        if (b2 > a1 || double_eq(b2, a1)) can_merge = true;
        if (can_merge && (
            (b1 > a2 && a1 < b2) || double_eq(b1, a2) || double_eq(b2, a1))) {
            double new_lower = std::min(a1, a2);
            double new_upper = std::max(b1, b2);
            bool new_lower_inc = (new_lower == a1) ? lower_inclusive : other_interval->lower_inclusive;
            bool new_upper_inc = (new_upper == b1) ? upper_inclusive : other_interval->upper_inclusive;
            return std::make_unique<IntervalSet>(new_lower, new_upper, new_lower_inc, new_upper_inc);
        }
    }
    // Otherwise, return a UnionSet
    return std::make_unique<UnionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> IntervalSet::intersect(const Set& other) const {
    auto other_interval = dynamic_cast<const IntervalSet*>(&other);
    if (other_interval) {
        double new_lower = std::max(lower_bound, other_interval->lower_bound);
        double new_upper = std::min(upper_bound, other_interval->upper_bound);
        bool new_lower_inc = (lower_bound < other_interval->lower_bound) ? other_interval->lower_inclusive : lower_inclusive;
        bool new_upper_inc = (upper_bound > other_interval->upper_bound) ? other_interval->upper_inclusive : upper_inclusive;
        if (new_lower < new_upper || (double_eq(new_lower, new_upper) && new_lower_inc && new_upper_inc)) {
            return std::make_unique<IntervalSet>(new_lower, new_upper, new_lower_inc, new_upper_inc);
        } else {
            return EmptySet::instance().clone();
        }
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
    else oss << lower_bound;
    oss << ", ";
    if (std::isinf(upper_bound)) oss << (upper_bound > 0 ? "inf" : "-inf");
    else oss << upper_bound;
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