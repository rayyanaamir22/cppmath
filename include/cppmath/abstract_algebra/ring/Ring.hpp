#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "RingElement.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract ring (R, +, *): an abelian group under +, associative *, and * distributes over +.
 *
 * @tparam Derived     the concrete ring class (CRTP), e.g. IntegerMod4.
 * @tparam ElementType its element class, e.g. IntegerMod4Element.
 */
template<typename Derived, typename ElementType>
class Ring : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Ring() = default;
    /** @brief The additive operation +. */
    virtual const AlgebraicOperation<ElementType, Derived>& addition_op() const = 0;
    /** @brief The multiplicative operation *. */
    virtual const AlgebraicOperation<ElementType, Derived>& multiplication_op() const = 0;
};

/** @brief Example concrete structures for tests and demos; not part of the abstract hierarchy. */
namespace example {

class IntegerMod4;
class IntegerMod4Element;
class Mod4Addition;
class Mod4Multiplication;

/** @brief Addition mod 4. Associative and commutative. */
class Mod4Addition : public AlgebraicOperation<IntegerMod4Element, IntegerMod4> {
public:
    Mod4Addition();
    IntegerMod4Element operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const override;
    bool is_associative(const IntegerMod4& ring) const override;
    bool is_commutative(const IntegerMod4& ring) const override;
};

/** @brief Multiplication mod 4. Associative and commutative. */
class Mod4Multiplication : public AlgebraicOperation<IntegerMod4Element, IntegerMod4> {
public:
    Mod4Multiplication();
    IntegerMod4Element operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const override;
    bool is_associative(const IntegerMod4& ring) const override;
    bool is_commutative(const IntegerMod4& ring) const override;
};

/**
 * @brief Example: the ring Z/4Z = {0, 1, 2, 3} with + and * mod 4.
 *
 * Not a field: 2 has no multiplicative inverse.
 */
class IntegerMod4 : public Ring<IntegerMod4, IntegerMod4Element> {
    Mod4Addition add_op_;
    Mod4Multiplication mul_op_;
public:
    IntegerMod4();
    const AlgebraicOperation<IntegerMod4Element, IntegerMod4>& addition_op() const override;
    const AlgebraicOperation<IntegerMod4Element, IntegerMod4>& multiplication_op() const override;
    /** @brief Returns 1, the *multiplicative* identity (the additive one is 0). */
    IntegerMod4Element identity() const override;
    /** @brief True if the element's value is in 0..3. */
    bool contains(const IntegerMod4Element& element) const override;
    /** @brief 0, 1, 2, 3. */
    std::vector<IntegerMod4Element> elements() const override;
};

/** @brief Example: an element of Z/4Z. */
class IntegerMod4Element : public RingElement<IntegerMod4Element, IntegerMod4> {
public:
    /**
     * @param value reduced with C++ `%`, so a negative value stays negative
     *              (and then fails IntegerMod4::contains).
     */
    IntegerMod4Element(std::shared_ptr<const IntegerMod4> parent, int value);
    /// The representative in 0..3.
    int int_value() const;
    /** @brief Sum mod 4, via the ring's addition operation. */
    IntegerMod4Element add(const IntegerMod4Element& other, const IntegerMod4& ring) const;
    /** @brief Product mod 4, via the ring's multiplication operation. */
    IntegerMod4Element multiply(const IntegerMod4Element& other, const IntegerMod4& ring) const;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 