#pragma once
#include "Set.hpp"
#include <set>
#include <vector>
#include <initializer_list>
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief A finite set of reals, e.g. {1, 2, 3}.
 *
 * Like every Set it is immutable in use: `with_element` / `without_element`
 * return a new set. Membership, equality, union, intersection and element removal
 * all compare with a 1e-9 tolerance, so values closer than that count as the same
 * element. (`elements` is a public member holding the exact doubles; treat it as read-only.)
 */
class FiniteSet : public Set {
public:
    /// The members, sorted ascending.
    std::set<double> elements;

    /** @brief The set of `vals` (duplicates collapse). */
    FiniteSet(std::initializer_list<double> vals = {});
    /** @brief The set of `vals` (duplicates collapse). */
    FiniteSet(const std::vector<double>& vals);

    bool contains(double value) const override;
    /** @brief With another FiniteSet: one merged FiniteSet (values within 1e-9 merge). Otherwise a UnionSet. */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief With another FiniteSet: the common elements (within 1e-9). Otherwise an IntersectionSet. */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    /** @brief ℝ with the points removed: a union of open intervals, or UniversalSet if the set is empty. */
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;

    /** @brief A new set that also contains `value` (a copy of this set if it already does). */
    std::unique_ptr<FiniteSet> with_element(double value) const;
    /** @brief A new set without `value` (and anything within 1e-9 of it). */
    std::unique_ptr<FiniteSet> without_element(double value) const;
    /// True if there are no elements.
    bool is_empty() const;
    /// Number of elements.
    size_t size() const;
};

} // namespace sets
} // namespace cppmath 