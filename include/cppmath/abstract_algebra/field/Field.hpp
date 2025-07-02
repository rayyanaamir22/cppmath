/*
 * Field.hpp
 * Abstract base for fields and example implementation for a finite field (mod p).
 * Includes abstract operations and example concrete classes for use in tests/examples.
 */
#ifndef CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELD_HPP
#define CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELD_HPP
#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "FieldElement.hpp" // is this not a circular dependency?

namespace cppmath {
namespace abstract_algebra {

// Abstract base for fields. Requires addition and multiplication operations.
// Concrete fields must implement these operations and provide them via addition_op() and multiplication_op().

// Forward declarations for operations
class Addition;
class Multiplication;

// Example: FiniteField and FiniteFieldElement for a prime field (mod p)
class FiniteField;
class FiniteFieldElement;

// Addition and Multiplication should be implemented as subclasses of AlgebraicOperation for the field.

template<typename ElementType>
class Field : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Field() = default;
    // Get the addition operation
    virtual const AlgebraicOperation<ElementType, Field<ElementType>>& addition_op() const = 0;
    // Get the multiplication operation
    virtual const AlgebraicOperation<ElementType, Field<ElementType>>& multiplication_op() const = 0;
    // Add field-specific virtual methods as needed
};

namespace example {

class Addition : public AlgebraicOperation<FiniteFieldElement, FiniteField> {
public:
    Addition();
    FiniteFieldElement operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const override;
    bool is_associative(const FiniteField& field) const override;
    bool is_commutative(const FiniteField& field) const override;
};

class Multiplication : public AlgebraicOperation<FiniteFieldElement, FiniteField> {
public:
    Multiplication();
    FiniteFieldElement operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const override;
    bool is_associative(const FiniteField& field) const override;
    bool is_commutative(const FiniteField& field) const override;
};

class FiniteField : public Field<FiniteFieldElement> {
    int p_;
    Addition add_op_;
    Multiplication mul_op_;
public:
    FiniteField(int p);
    int modulus() const;
    const AlgebraicOperation<FiniteFieldElement, FiniteField>& addition_op() const override;
    const AlgebraicOperation<FiniteFieldElement, FiniteField>& multiplication_op() const override;
    FiniteFieldElement identity() const override;
    bool contains(const FiniteFieldElement& element) const override;
    std::vector<FiniteFieldElement> elements() const override;
};

class FiniteFieldElement : public FieldElement<FiniteFieldElement, FiniteField> {
public:
    FiniteFieldElement(std::shared_ptr<const FiniteField> parent, int value);
    int int_value() const;
    FiniteFieldElement add(const FiniteFieldElement& other, const FiniteField& field) const override;
    FiniteFieldElement multiply(const FiniteFieldElement& other, const FiniteField& field) const override;
    FiniteFieldElement inverse(const FiniteField& field) const override;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath
#endif // CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELD_HPP 