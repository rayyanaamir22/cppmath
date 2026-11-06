#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

/**
 * @brief Abstract element of a vector space: supports addition and scalar multiplication.
 *
 * @tparam Derived       the concrete vector class (CRTP).
 * @tparam StructureType the vector space class it belongs to.
 *
 * The inherited `operate()` means **addition**, and `value()` is always 0.
 */
template<typename Derived, typename StructureType>
class VectorElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~VectorElement() = default;
    /** @brief `*this + other` in `structure`. */
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    /** @brief `scalar * *this` in `structure`. */
    virtual Derived scalar_multiply(double scalar, const StructureType& structure) const = 0;
    virtual int value() const override { return 0; } // Not meaningful for all vectors
    virtual bool equals(const Derived& other) const override = 0;
    /** @brief The primary operation, which for a vector space is addition: same as `add()`. */
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->add(other, structure);
    }
};

} // namespace abstract_algebra
} // namespace cppmath 