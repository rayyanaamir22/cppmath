#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

template<typename Derived, typename StructureType>
class VectorElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~VectorElement() = default;
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    virtual Derived scalar_multiply(double scalar, const StructureType& structure) const = 0;
    virtual int value() const override { return 0; } // Not meaningful for all vectors
    virtual bool equals(const Derived& other) const override = 0;
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->add(other, structure);
    }
};

} // namespace abstract_algebra
} // namespace cppmath 