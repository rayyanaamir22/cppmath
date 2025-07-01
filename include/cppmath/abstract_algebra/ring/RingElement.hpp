#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract RingElement: Derived is the concrete element, StructureType is the ring type
// Inherit and implement for concrete ring elements

template<typename Derived, typename StructureType>
class RingElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~RingElement() = default;
    // Add two ring elements
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    // Multiply two ring elements
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    // Add more ring element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 