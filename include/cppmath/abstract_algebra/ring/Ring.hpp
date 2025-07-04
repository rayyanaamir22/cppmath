#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "RingElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract Ring structure: Derived is the concrete ring, ElementType is the type of elements in the ring
// Inherit and implement for concrete rings

template<typename Derived, typename ElementType>
class Ring : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Ring() = default;
    // Get the addition operation
    virtual const AlgebraicOperation<ElementType, Derived>& addition_op() const = 0;
    // Get the multiplication operation
    virtual const AlgebraicOperation<ElementType, Derived>& multiplication_op() const = 0;
    // Add ring-specific virtual methods as needed
};

namespace example {

class IntegerMod4;
class IntegerMod4Element;
class Mod4Addition;
class Mod4Multiplication;

class Mod4Addition : public AlgebraicOperation<IntegerMod4Element, IntegerMod4> {
public:
    Mod4Addition();
    IntegerMod4Element operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const override;
    bool is_associative(const IntegerMod4& ring) const override;
    bool is_commutative(const IntegerMod4& ring) const override;
};

class Mod4Multiplication : public AlgebraicOperation<IntegerMod4Element, IntegerMod4> {
public:
    Mod4Multiplication();
    IntegerMod4Element operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const override;
    bool is_associative(const IntegerMod4& ring) const override;
    bool is_commutative(const IntegerMod4& ring) const override;
};

class IntegerMod4 : public Ring<IntegerMod4, IntegerMod4Element> {
    Mod4Addition add_op_;
    Mod4Multiplication mul_op_;
public:
    IntegerMod4();
    const AlgebraicOperation<IntegerMod4Element, IntegerMod4>& addition_op() const override;
    const AlgebraicOperation<IntegerMod4Element, IntegerMod4>& multiplication_op() const override;
    IntegerMod4Element identity() const override;
    bool contains(const IntegerMod4Element& element) const override;
    std::vector<IntegerMod4Element> elements() const override;
};

class IntegerMod4Element : public RingElement<IntegerMod4Element, IntegerMod4> {
public:
    IntegerMod4Element(std::shared_ptr<const IntegerMod4> parent, int value);
    int int_value() const;
    IntegerMod4Element add(const IntegerMod4Element& other, const IntegerMod4& ring) const;
    IntegerMod4Element multiply(const IntegerMod4Element& other, const IntegerMod4& ring) const;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 