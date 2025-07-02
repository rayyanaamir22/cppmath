// Implementation for Field class

#include "../../../include/cppmath/abstract_algebra/field/Field.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

using namespace cppmath::abstract_algebra;
using namespace cppmath::abstract_algebra::example;

// --- Addition ---
Addition::Addition() : AlgebraicOperation("Addition", 2) {}
FiniteFieldElement Addition::operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const {
    int sum = (a.int_value() + b.int_value()) % field.modulus();
    return FiniteFieldElement(a.parent_structure(), sum);
}
bool Addition::is_associative(const FiniteField&) const { return true; }
bool Addition::is_commutative(const FiniteField&) const { return true; }

// --- Multiplication ---
Multiplication::Multiplication() : AlgebraicOperation("Multiplication", 2) {}
FiniteFieldElement Multiplication::operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const {
    int prod = (a.int_value() * b.int_value()) % field.modulus();
    return FiniteFieldElement(a.parent_structure(), prod);
}
bool Multiplication::is_associative(const FiniteField&) const { return true; }
bool Multiplication::is_commutative(const FiniteField&) const { return true; }

// --- FiniteField ---
FiniteField::FiniteField(int p) : Field("FiniteField", p), p_(p), add_op_(), mul_op_() {}
int FiniteField::modulus() const { return p_; }
const AlgebraicOperation<FiniteFieldElement, FiniteField>& FiniteField::addition_op() const { return add_op_; }
const AlgebraicOperation<FiniteFieldElement, FiniteField>& FiniteField::multiplication_op() const { return mul_op_; }
FiniteFieldElement FiniteField::identity() const { return FiniteFieldElement(std::make_shared<FiniteField>(*this), 1); }
bool FiniteField::contains(const FiniteFieldElement& element) const {
    int v = element.int_value();
    return v >= 0 && v < p_;
}
std::vector<FiniteFieldElement> FiniteField::elements() const {
    std::vector<FiniteFieldElement> elems;
    auto self = std::make_shared<FiniteField>(*this);
    for (int i = 0; i < p_; ++i) {
        elems.emplace_back(self, i);
    }
    return elems;
}

// --- FiniteFieldElement ---
FiniteFieldElement::FiniteFieldElement(std::shared_ptr<const FiniteField> parent, int value)
    : FieldElement(parent, value % parent->modulus()) {}
int FiniteFieldElement::int_value() const { return static_cast<int>(value_); }
FiniteFieldElement FiniteFieldElement::add(const FiniteFieldElement& other, const FiniteField& field) const {
    return field.addition_op().operate(*this, other, field);
}
FiniteFieldElement FiniteFieldElement::multiply(const FiniteFieldElement& other, const FiniteField& field) const {
    return field.multiplication_op().operate(*this, other, field);
}
FiniteFieldElement FiniteFieldElement::inverse(const FiniteField& field) const {
    int a = int_value(), m = field.modulus();
    int t = 0, newt = 1, r = m, newr = a;
    while (newr != 0) {
        int quotient = r / newr;
        int temp = t;
        t = newt;
        newt = temp - quotient * newt;
        temp = r;
        r = newr;
        newr = temp - quotient * newr;
    }
    if (r > 1) throw std::runtime_error("Element has no inverse");
    if (t < 0) t += m;
    return FiniteFieldElement(this->parent_structure(), t);
} 