#pragma once

namespace cppmath {
namespace abstract_algebra {

template<typename Derived, typename StructureType>
class AlgebraicElement {
public:
    virtual ~AlgebraicElement() = default;
    // Abstract method for element-specific logic (e.g., operation with another element)
    virtual Derived operate(const Derived& other, const StructureType& structure) const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 