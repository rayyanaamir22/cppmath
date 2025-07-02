/*
 * FieldElement.hpp
 * Abstract base for field elements and example implementation for a finite field element (mod p).
 * Includes abstract operations and example concrete classes for use in tests/examples.
 */
#ifndef CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELDELEMENT_HPP
#define CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELDELEMENT_HPP
#pragma once
#include "../AlgebraicElement.hpp"
#include "../AlgebraicOperation.hpp"
#include "Field.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract base for field elements. Concrete elements should use the field's addition and multiplication operations.

// Abstract FieldElement: Derived is the concrete element, StructureType is the field type
// Inherit and implement for concrete field elements

template<typename Derived, typename StructureType>
class FieldElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    FieldElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~FieldElement() = default;
    // Add two field elements
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    // Multiply two field elements
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    // Return the multiplicative inverse
    virtual Derived inverse(const StructureType& structure) const = 0;
    // Add more field element-specific virtual methods as needed
    virtual int value() const override { return value_; }
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return structure.addition_op().operate(static_cast<const Derived&>(*this), other, structure);
    }
    // Note: operate() is addition by default, but can be overridden for other operations if needed.
};

// Note: Concrete FieldElement implementations should use the addition and multiplication operations from their parent Field structure.

// Forward declarations for example
namespace example {
class FiniteFieldElement;
}

} // namespace abstract_algebra
} // namespace cppmath
#endif // CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELDELEMENT_HPP 