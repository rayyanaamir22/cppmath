#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract RingElement: Derived is the concrete element, StructureType is the ring type
// Inherit and implement for concrete ring elements

template<typename Derived, typename StructureType>
class RingElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    RingElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~RingElement() = default;
    // Add two ring elements
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    // Multiply two ring elements
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    virtual int value() const override { return value_; }
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->add(other, structure);
    }
    // Add more ring element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 