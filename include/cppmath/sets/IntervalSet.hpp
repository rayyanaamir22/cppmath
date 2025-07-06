#pragma once
#include "Set.hpp"
#include <limits>
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class IntervalSet : public Set {
public:
    double lower_bound, upper_bound;
    bool lower_inclusive, upper_inclusive;

    IntervalSet(double lower, double upper, bool lower_inc, bool upper_inc);

    static std::unique_ptr<IntervalSet> open(double lower, double upper);
    static std::unique_ptr<IntervalSet> closed(double lower, double upper);
    static std::unique_ptr<IntervalSet> open_closed(double lower, double upper);
    static std::unique_ptr<IntervalSet> closed_open(double lower, double upper);
    static std::unique_ptr<IntervalSet> positive_real();
    static std::unique_ptr<IntervalSet> negative_real();
    static std::unique_ptr<IntervalSet> non_negative_real();
    static std::unique_ptr<IntervalSet> non_positive_real();
    static std::unique_ptr<IntervalSet> all_real();

    bool contains(double value) const override;
    std::unique_ptr<Set> set_union(const Set& other) const override;
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 