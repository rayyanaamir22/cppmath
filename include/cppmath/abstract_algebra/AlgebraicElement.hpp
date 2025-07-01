#pragma once
#include <memory>

namespace cppmath {
namespace abstract_algebra {

template<typename Derived, typename StructureType>
class AlgebraicElement {
protected:
    std::shared_ptr<const StructureType> parent_structure_;
public:
    AlgebraicElement(std::shared_ptr<const StructureType> parent_structure = nullptr)
        : parent_structure_(parent_structure) {}
    virtual ~AlgebraicElement() = default;
    // Get the parent structure
    virtual std::shared_ptr<const StructureType> parent_structure() const { return parent_structure_; }
    // Abstract: check equality
    virtual bool equals(const Derived& other) const = 0;
    // Abstract: get value (to be defined by derived class)
    virtual auto value() const -> decltype(auto) = 0;
    // Abstract: operate with another element
    virtual Derived operate(const Derived& other, const StructureType& structure) const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 