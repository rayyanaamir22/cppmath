#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

template<typename Derived, typename StructureType>
class LieElement : public AlgebraicElement<Derived, StructureType> {
public:
    using AlgebraicElement<Derived, StructureType>::AlgebraicElement;
    virtual ~LieElement() = default;
    // Lie bracket
    virtual Derived bracket(const Derived& other, const StructureType& structure) const = 0;
    virtual int value() const override { return 0; } // Not meaningful for all Lie elements
    virtual bool equals(const Derived& other) const override = 0;
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->bracket(other, structure);
    }
};

} // namespace abstract_algebra
} // namespace cppmath 