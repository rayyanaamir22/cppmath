#pragma once
#include "Set.hpp"
#include <limits>
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief An interval of reals with open or closed ends, e.g. [0, 1) or (-inf, 0].
 *
 * Bounds may be ±infinity (an infinite end should be open). The constructor
 * doesn't check that lower <= upper. `toString()` prints `[a, b)` style, with
 * `-inf` / `inf` for infinite ends.
 */
class IntervalSet : public Set {
public:
    /// The endpoints; may be -infinity / +infinity.
    double lower_bound, upper_bound;
    /// Whether each endpoint belongs to the interval (closed end).
    bool lower_inclusive, upper_inclusive;

    /**
     * @param lower     lower endpoint (lower <= upper is not checked).
     * @param upper     upper endpoint.
     * @param lower_inc whether the lower endpoint is included.
     * @param upper_inc whether the upper endpoint is included.
     */
    IntervalSet(double lower, double upper, bool lower_inc, bool upper_inc);

    /** @brief (lower, upper) */
    static std::unique_ptr<IntervalSet> open(double lower, double upper);
    /** @brief [lower, upper] */
    static std::unique_ptr<IntervalSet> closed(double lower, double upper);
    /** @brief (lower, upper] */
    static std::unique_ptr<IntervalSet> open_closed(double lower, double upper);
    /** @brief [lower, upper) */
    static std::unique_ptr<IntervalSet> closed_open(double lower, double upper);
    /** @brief (0, inf) */
    static std::unique_ptr<IntervalSet> positive_real();
    /** @brief (-inf, 0) */
    static std::unique_ptr<IntervalSet> negative_real();
    /** @brief [0, inf) */
    static std::unique_ptr<IntervalSet> non_negative_real();
    /** @brief (-inf, 0] */
    static std::unique_ptr<IntervalSet> non_positive_real();
    /** @brief (-inf, inf). A different object from UniversalSet, and not `equals()` to it. */
    static std::unique_ptr<IntervalSet> all_real();

    bool contains(double value) const override;
    /**
     * @brief One merged interval when `other` is an interval that overlaps this one, or meets it at
     *        a point that at least one of them contains; otherwise a UnionSet.
     *
     * So [0, 1] U (1, 2) is [0, 2), but (0, 1) U (1, 2) stays a UnionSet because 1 is in neither.
     * An endpoint shared by both is included if either includes it.
     */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /**
     * @brief With another interval: the overlap, or EmptySet if there is none. Otherwise an IntersectionSet.
     *
     * An endpoint shared by both is included only if both include it.
     */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    /** @brief The part of ℝ outside the interval: up to two intervals (a UnionSet), one interval, or EmptySet. */
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 