#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract GroupElement: Derived is the concrete element, StructureType is the group type
// Inherit and implement for concrete group elements

template<typename Derived, typename StructureType>
class GroupElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    GroupElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~GroupElement() = default;
    // Return the inverse of the element
    virtual Derived inverse(const StructureType& structure) const = 0;
    // Return the identity element
    virtual Derived identity(const StructureType& structure) const = 0;
    // Operate with another element (group operation)
    virtual Derived operate(const Derived& other, const StructureType& structure) const = 0;
    virtual int value() const override { return value_; }
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
};

} // namespace abstract_algebra
} // namespace cppmath 