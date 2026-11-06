// Implementation for Field class

#include "../../../include/cppmath/abstract_algebra/field/Field.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

using namespace cppmath::abstract_algebra;
using namespace cppmath::abstract_algebra::example;

// --- Addition ---
Addition::Addition() : cppmath::abstract_algebra::AlgebraicOperation<FiniteFieldElement, FiniteField>("Addition", 2) {} // base class AlgebraicOperation constructor for Addition
FiniteFieldElement Addition::operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const { // Implements operate for Addition
    int sum = (a.int_value() + b.int_value()) % field.modulus(); // Computes sum modulo field modulus
    return FiniteFieldElement(a.parent_structure(), sum); // Returns new FiniteFieldElement with computed sum
}
bool Addition::is_associative(const FiniteField&) const { return true; }
bool Addition::is_commutative(const FiniteField&) const { return true; }
bool Addition::has_identity(const FiniteField&) const { return true; }
FiniteFieldElement Addition::identity(const FiniteField& field) const {
    return FiniteFieldElement(std::make_shared<FiniteField>(field), 0);
}

// --- Multiplication ---
Multiplication::Multiplication() : cppmath::abstract_algebra::AlgebraicOperation<FiniteFieldElement, FiniteField>("Multiplication", 2) {}
FiniteFieldElement Multiplication::operate(const FiniteFieldElement& a, const FiniteFieldElement& b, const FiniteField& field) const {
    int prod = (a.int_value() * b.int_value()) % field.modulus();
    return FiniteFieldElement(a.parent_structure(), prod);
}
bool Multiplication::is_associative(const FiniteField&) const { return true; }
bool Multiplication::is_commutative(const FiniteField&) const { return true; }
bool Multiplication::has_identity(const FiniteField&) const { return true; }
FiniteFieldElement Multiplication::identity(const FiniteField& field) const {
    return FiniteFieldElement(std::make_shared<FiniteField>(field), 1);
}

// --- FiniteField ---
namespace {
// Checked before the base class is built, because Field's `order` is initialised from p.
int checked_prime(int p) {
    if (p < 2) throw std::invalid_argument("FiniteField: modulus must be a prime, got " + std::to_string(p));
    for (long long d = 2; d * d <= p; ++d)  // trial division; fine for the small moduli used here
        if (p % d == 0) throw std::invalid_argument("FiniteField: modulus must be a prime, got " + std::to_string(p));
    return p;
}

// Validates the parent, then shifts `value` into 0..p-1 (C++ `%` keeps the sign of the dividend).
int reduce_mod(const std::shared_ptr<const FiniteField>& parent, int value) {
    if (!parent) throw std::invalid_argument("FiniteFieldElement: parent field must not be null");
    int m = parent->modulus();
    return ((value % m) + m) % m;
}
} // namespace

FiniteField::FiniteField(int p) : Field("FiniteField", checked_prime(p)), p_(p), add_op_(), mul_op_() {}
int FiniteField::modulus() const { return p_; }
const AlgebraicOperation<FiniteFieldElement, FiniteField>& FiniteField::addition_op() const { return add_op_; }
const AlgebraicOperation<FiniteFieldElement, FiniteField>& FiniteField::multiplication_op() const { return mul_op_; }
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
    : FieldElement(parent, reduce_mod(parent, value)) {}
int FiniteFieldElement::int_value() const { return value_; }
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
    if (r > 1) throw std::invalid_argument("FiniteFieldElement: " + std::to_string(a) + " has no inverse mod " + std::to_string(m));
    if (t < 0) t += m;
    return FiniteFieldElement(this->parent_structure(), t);
} 