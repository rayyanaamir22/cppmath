#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief The union (A U B) of two owned sets, used when they can't be merged into a simpler set.
 *
 * Printed as `(A U B)`. Operations push through by set identities:
 * (A U B) ∩ C = (A ∩ C) U (B ∩ C) and (A U B)^c = A^c ∩ B^c.
 */
class UnionSet : public Set {
public:
    /// Left operand (owned).
    std::unique_ptr<Set> set1;
    /// Right operand (owned).
    std::unique_ptr<Set> set2;

    /** @brief Takes ownership of both operands. They must not be null. */
    UnionSet(std::unique_ptr<Set> s1, std::unique_ptr<Set> s2);

    bool contains(double value) const override;
    /** @brief A new UnionSet ((A U B) U C) wrapping copies; no simplification. */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief Distributes: (A ∩ C) U (B ∩ C). */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    /** @brief De Morgan: A^c ∩ B^c. */
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 