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


// (Removed global forward declarations for Addition, Multiplication, FiniteField, FiniteFieldElement)


/**
 * @brief Abstract field (F, +, *): a commutative ring where every nonzero element has a multiplicative inverse.
 *
 * @tparam Derived     the concrete field class (CRTP), e.g. FiniteField.
 * @tparam ElementType its element class, e.g. FiniteFieldElement.
 */
template<typename Derived, typename ElementType>
class Field : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Field() = default;
    /** @brief The additive operation +. */
    virtual const AlgebraicOperation<ElementType, Derived>& addition_op() const = 0;
    /** @brief The multiplicative operation *. */
    virtual const AlgebraicOperation<ElementType, Derived>& multiplication_op() const = 0;
};

/** @brief Example concrete structures for tests and demos; not part of the abstract hierarchy. */
namespace example {

class FiniteField;

/** @brief Addition mod p in a FiniteField. Associative and commutative. */
class Addition : public AlgebraicOperation<FiniteFieldElement, FiniteField> {
public:
    Addition();
    FiniteFieldElement operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const override;
    bool is_associative(const FiniteField& field) const override;
    bool is_commutative(const FiniteField& field) const override;
};

/** @brief Multiplication mod p in a FiniteField. Associative and commutative. */
class Multiplication : public AlgebraicOperation<FiniteFieldElement, FiniteField> {
public:
    Multiplication();
    FiniteFieldElement operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const override;
    bool is_associative(const FiniteField& field) const override;
    bool is_commutative(const FiniteField& field) const override;
};

/**
 * @brief Example: integers mod p, written Z/pZ or F_p.
 *
 * This is a field only when p is prime. The constructor does **not** check
 * primality; for composite p some nonzero elements have no inverse, and
 * FiniteFieldElement::inverse throws for them.
 */
class FiniteField : public Field<FiniteField, FiniteFieldElement> {
    int p_;
    Addition add_op_;
    Multiplication mul_op_;
public:
    /** @param p the modulus (should be prime; not checked). */
    FiniteField(int p);
    /// The modulus p.
    int modulus() const;
    const AlgebraicOperation<FiniteFieldElement, FiniteField>& addition_op() const override;
    const AlgebraicOperation<FiniteFieldElement, FiniteField>& multiplication_op() const override;
    /** @brief Returns 1, the *multiplicative* identity (the additive one is 0). */
    FiniteFieldElement identity() const override;
    /** @brief True if the element's value is in 0..p-1. */
    bool contains(const FiniteFieldElement& element) const override;
    /** @brief 0, 1, ..., p-1. */
    std::vector<FiniteFieldElement> elements() const override;
};

/** @brief Example: an element of Z/pZ. */
class FiniteFieldElement : public FieldElement<FiniteFieldElement, FiniteField> {
public:
    /**
     * @param parent the field; must not be null (its modulus is read here).
     * @param value  reduced with C++ `%`, so a negative value stays negative
     *               (and then fails FiniteField::contains).
     */
    FiniteFieldElement(std::shared_ptr<const FiniteField> parent, int value);
    /// The representative in 0..p-1.
    int int_value() const;
    /** @brief Sum mod p, via the field's addition operation. */
    FiniteFieldElement add(const FiniteFieldElement& other, const FiniteField& field) const override;
    /** @brief Product mod p, via the field's multiplication operation. */
    FiniteFieldElement multiply(const FiniteFieldElement& other, const FiniteField& field) const override;
    /**
     * @brief Multiplicative inverse mod p, found with the extended Euclidean algorithm.
     * @throws std::runtime_error if the element has no inverse (0, or a non-unit when p is composite).
     */
    FiniteFieldElement inverse(const FiniteField& field) const override;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath
#endif // CPPMATH_ABSTRACT_ALGEBRA_FIELD_FIELD_HPP 