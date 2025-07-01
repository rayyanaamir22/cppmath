#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract GroupElement: Derived is the concrete element, StructureType is the group type
// Inherit and implement for concrete group elements

template<typename Derived, typename StructureType>
class GroupElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~GroupElement() = default;
    // Return the inverse of the element
    virtual Derived inverse() const = 0;
    // Return the identity element
    virtual Derived identity() const = 0;
    // Operate with another element (group operation)
    virtual Derived operate(const Derived& other, const StructureType& structure) const = 0;
    // Add more group element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 