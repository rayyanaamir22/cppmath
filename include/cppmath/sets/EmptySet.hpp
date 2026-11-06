#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief The empty set ∅. Contains nothing; its complement is UniversalSet.
 *
 * `instance()` is the shared canonical object, but the class is copyable
 * (`clone()` returns a copy). All EmptySets are `equals()` to each other.
 * Note that an empty FiniteSet is a different object and is not `equals()` to it.
 */
class EmptySet : public Set {
private:
    EmptySet() = default;
public:
    /// Copyable so that `clone()` can return an independent object; `instance()` is the shared one.
    EmptySet(const EmptySet&) = default;
    /** @brief The canonical shared instance. */
    static EmptySet& instance();

    bool contains(double value) const override;
    /** @brief ∅ U A = A: a copy of `other`. */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief ∅ ∩ A = ∅. */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 