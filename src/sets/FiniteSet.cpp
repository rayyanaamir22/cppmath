#include "cppmath/sets/FiniteSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/utils/format.hpp"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>

namespace cppmath {
namespace sets {

namespace {
constexpr double EPSILON = 1e-9;
bool double_eq(double a, double b) {
    return a == b || std::fabs(a - b) < EPSILON;
}
}

FiniteSet::FiniteSet(std::initializer_list<double> vals) : elements(vals) {}
FiniteSet::FiniteSet(const std::vector<double>& vals) : elements(vals.begin(), vals.end()) {}

bool FiniteSet::contains(double value) const {
    for (double v : elements) {
        if (double_eq(v, value)) return true;
    }
    return false;
}

std::unique_ptr<Set> FiniteSet::set_union(const Set& other) const {
    auto other_finite = dynamic_cast<const FiniteSet*>(&other);
    if (other_finite) {
        // Skip values already present within tolerance, so {1} U {1 + 1e-12} stays {1}.
        std::vector<double> result(elements.begin(), elements.end());
        for (double v : other_finite->elements)
            if (!contains(v)) result.push_back(v);
        return std::make_unique<FiniteSet>(result);
    }
    return std::make_unique<UnionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> FiniteSet::intersect(const Set& other) const {
    auto other_finite = dynamic_cast<const FiniteSet*>(&other);
    if (other_finite) {
        // Same tolerance as contains(): keep our values that the other set also contains.
        std::vector<double> result;
        for (double v : elements)
            if (other_finite->contains(v)) result.push_back(v);
        return std::make_unique<FiniteSet>(result);
    }
    return std::make_unique<IntersectionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> FiniteSet::complement() const {
    if (elements.empty()) {
        return UniversalSet::instance().clone();
    }
    std::vector<double> sorted_elements(elements.begin(), elements.end());
    std::sort(sorted_elements.begin(), sorted_elements.end());
    std::vector<std::unique_ptr<Set>> intervals;
    double prev = -std::numeric_limits<double>::infinity();
    for (double v : sorted_elements) {
        intervals.push_back(std::make_unique<IntervalSet>(prev, v, false, false));
        prev = v;
    }
    intervals.push_back(std::make_unique<IntervalSet>(prev, std::numeric_limits<double>::infinity(), false, false));
    // Build union of all intervals
    std::unique_ptr<Set> result = std::move(intervals[0]);
    for (size_t i = 1; i < intervals.size(); ++i) {
        result = std::make_unique<UnionSet>(std::move(result), std::move(intervals[i]));
    }
    return result;
}

std::string FiniteSet::toString() const {
    std::ostringstream oss;
    oss << "{";
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        if (it != elements.begin()) oss << ", ";
        oss << utils::format_number(*it);
    }
    oss << "}";
    return oss.str();
}

std::unique_ptr<Set> FiniteSet::clone() const {
    return std::make_unique<FiniteSet>(std::vector<double>(elements.begin(), elements.end()));
}

bool FiniteSet::equals(const Set& other) const {
    auto o = dynamic_cast<const FiniteSet*>(&other);
    if (!o) return false;
    if (elements.size() != o->elements.size()) return false;
    auto it1 = elements.begin(), it2 = o->elements.begin();
    for (; it1 != elements.end(); ++it1, ++it2) {
        if (!double_eq(*it1, *it2)) return false;
    }
    return true;
}

std::unique_ptr<FiniteSet> FiniteSet::with_element(double value) const {
    std::vector<double> result(elements.begin(), elements.end());
    if (!contains(value)) result.push_back(value);
    return std::make_unique<FiniteSet>(result);
}

std::unique_ptr<FiniteSet> FiniteSet::without_element(double value) const {
    std::vector<double> result;
    for (double v : elements)
        if (!double_eq(v, value)) result.push_back(v);
    return std::make_unique<FiniteSet>(result);
}

bool FiniteSet::is_empty() const {
    return elements.empty();
}
size_t FiniteSet::size() const {
    return elements.size();
}

} // namespace sets
} // namespace cppmath 