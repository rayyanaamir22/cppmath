#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

/**
 * @brief Abstract element of a Lie algebra.
 *
 * @tparam Derived       the concrete element class (CRTP).
 * @tparam StructureType the Lie algebra class it belongs to.
 *
 * The inherited `operate()` means the **bracket**, and `value()` is always 0.
 */
template<typename Derived, typename StructureType>
class LieElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~LieElement() = default;
    /** @brief The Lie bracket [*this, other] in `structure`. */
    virtual Derived bracket(const Derived& other, const StructureType& structure) const = 0;
    virtual int value() const override { return 0; } // Not meaningful for all Lie elements
    virtual bool equals(const Derived& other) const override = 0;
    /** @brief The primary operation, which for a Lie algebra is the bracket: same as `bracket()`. */
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->bracket(other, structure);
    }
};

} // namespace abstract_algebra
} // namespace cppmath 