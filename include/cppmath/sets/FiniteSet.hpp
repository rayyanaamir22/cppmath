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
 * Exception to the immutability rule: `add_element` / `remove_element` modify
 * the set in place. `contains`, `equals` and `remove_element` compare with a
 * 1e-9 tolerance, but `elements` stores exact doubles.
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
    /** @brief With another FiniteSet: one merged FiniteSet. Otherwise a UnionSet. */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief With another FiniteSet: the common elements, compared exactly. Otherwise an IntersectionSet. */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    /** @brief ℝ with the points removed: a union of open intervals, or UniversalSet if the set is empty. */
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;

    /** @brief Insert `value` in place (mutates this set). */
    void add_element(double value);
    /** @brief Remove the first element within 1e-9 of `value`, in place (mutates this set). */
    void remove_element(double value);
    /// True if there are no elements.
    bool is_empty() const;
    /// Number of elements.
    size_t size() const;
};

} // namespace sets
} // namespace cppmath 