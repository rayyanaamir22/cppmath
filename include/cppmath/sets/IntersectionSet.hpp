#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

/**
 * @brief The intersection (A ∩ B) of two owned sets, used when they can't be simplified.
 *
 * Printed as `(A ∩ B)`. Operations push through by set identities:
 * (A ∩ B) U C = (A U C) ∩ (B U C) and (A ∩ B)^c = A^c U B^c.
 */
class IntersectionSet : public Set {
public:
    /// Left operand (owned).
    std::unique_ptr<Set> set1;
    /// Right operand (owned).
    std::unique_ptr<Set> set2;

    /** @brief Takes ownership of both operands. They must not be null. */
    IntersectionSet(std::unique_ptr<Set> s1, std::unique_ptr<Set> s2);

    bool contains(double value) const override;
    /** @brief Distributes: (A U C) ∩ (B U C). */
    std::unique_ptr<Set> set_union(const Set& other) const override;
    /** @brief A new IntersectionSet ((A ∩ B) ∩ C) wrapping copies; no simplification. */
    std::unique_ptr<Set> intersect(const Set& other) const override;
    /** @brief De Morgan: A^c U B^c. */
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 