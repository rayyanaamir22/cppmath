#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief The universal set: the whole real line ℝ. Contains every double; its complement is EmptySet.
 *
 * `instance()` is the shared canonical object, but the class is copyable
 * (`clone()` returns a copy). All UniversalSets are `equals()` to each other,
 * but not to `IntervalSet::all_real()`.
 */
class UniversalSet : public Set {
private:
    UniversalSet() = default;
public:
    /** @brief The canonical shared instance. */
    static UniversalSet& instance();

    bool contains(double value) const override;
    /** @brief ℝ U A = ℝ. */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief ℝ ∩ A = A: a copy of `other`. */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
    /// Copyable so that `clone()` can return an independent object; `instance()` is the shared one.
    UniversalSet(const UniversalSet&) = default;
};

} // namespace sets
} // namespace cppmath 