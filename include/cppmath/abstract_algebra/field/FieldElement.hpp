#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract FieldElement: Derived is the concrete element, StructureType is the field type
// Inherit and implement for concrete field elements

template<typename Derived, typename StructureType>
class FieldElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~FieldElement() = default;
    // Add two field elements
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    // Multiply two field elements
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    // Return the multiplicative inverse
    virtual Derived inverse(const StructureType& structure) const = 0;
    // Add more field element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 