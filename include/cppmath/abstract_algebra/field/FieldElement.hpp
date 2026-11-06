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



/**
 * @brief Abstract element of a field: supports +, * and multiplicative inverse.
 *
 * @tparam Derived       the concrete element class (CRTP).
 * @tparam StructureType the field class it belongs to; must provide `addition_op()`.
 *
 * Stores an integer `value_`; `equals()` compares it. The inherited `operate()`
 * means **addition**, through the parent field's `addition_op()`.
 */
template<typename Derived, typename StructureType>
class FieldElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    /** @param value integer representative. */
    FieldElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~FieldElement() = default;
    /** @brief `*this + other` in `structure`. */
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    /** @brief `*this * other` in `structure`. */
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    /** @brief Multiplicative inverse; concrete types throw for 0. */
    virtual Derived inverse(const StructureType& structure) const = 0;
    virtual int value() const override { return value_; }
    /** @brief Compares integer values. */
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
    /** @brief The primary operation, which for a field is addition: `structure.addition_op()`. */
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