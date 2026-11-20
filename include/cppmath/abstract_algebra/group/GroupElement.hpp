#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract element of a group: supports the operation, inverse and identity.
 *
 * @tparam Derived       the concrete element class (CRTP).
 * @tparam StructureType the group class it belongs to.
 *
 * Stores an integer `value_`; the default `equals()` compares it. Elements that
 * aren't integers (e.g. permutations) override `equals()`.
 */
template<typename Derived, typename StructureType>
class GroupElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    /** @param value integer representative (ignored by non-integer groups). */
    GroupElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~GroupElement() = default;
    /** @brief The inverse g^{-1}, with g * g^{-1} = identity. */
    virtual Derived inverse(const StructureType& structure) const = 0;
    /** @brief The identity element of `structure`. */
    virtual Derived identity(const StructureType& structure) const = 0;
    /** @brief The group operation `*this * other`. */
    virtual Derived operate(const Derived& other, const StructureType& structure) const override = 0;
    virtual int value() const override { return value_; }
    /** @brief Compares integer values; override for non-integer elements. */
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
};

} // namespace abstract_algebra
} // namespace cppmath 