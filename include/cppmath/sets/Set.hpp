#pragma once
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief Abstract subset of the real line, manipulated polymorphically through `std::unique_ptr<Set>`.
 *
 * Contract for every subclass:
 *  - **Immutable operations:** `set_union`, `intersect` and `complement` return
 *    a *new* set and never modify `*this` or their argument.
 *  - **Simplify when possible:** combining two sets of the same kind may give a
 *    simpler set (two overlapping intervals merge into one); otherwise the result
 *    is a composite UnionSet / IntersectionSet.
 *  - **Canonical text:** `toString()` is fully parenthesised and meant to be
 *    parseable later: `[0, 1)`, `{1, 2}`, `(A U B)`, `(A ∩ B)`, `∅`, `ℝ`.
 */
class Set {
public:
    virtual ~Set() = default;
    /** @brief Membership test: whether `value` is in the set. Boundaries are compared with a 1e-9 tolerance. */
    virtual bool contains(double value) const = 0;
    /** @brief A new set equal to this ∪ other. */
    virtual std::unique_ptr<Set> set_union(const Set& other) const = 0;
    /** @brief A new set equal to this ∩ other. */
    virtual std::unique_ptr<Set> intersect(const Set& other) const = 0;
    /** @brief A new set equal to ℝ minus this (the complement within the real line). */
    virtual std::unique_ptr<Set> complement() const = 0;
    /** @brief Canonical text form, e.g. `([0, 1) U {2})`. */
    virtual std::string toString() const = 0;
    /** @brief Deep copy. */
    virtual std::unique_ptr<Set> clone() const = 0;
    /**
     * @brief **Structural** equality: same kind of set with equal parts (numbers within 1e-9).
     *
     * Not full set-theoretic equality: `IntervalSet::all_real()` does not equal
     * `UniversalSet`, and `[0, 1] U [1, 2]` as a UnionSet does not equal `[0, 2]`.
     * Unions and intersections match their operands in either order.
     */
    virtual bool equals(const Set& other) const = 0;
};

} // namespace sets
} // namespace cppmath 