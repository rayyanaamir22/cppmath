// Implementation for Ring class
// ... existing code ... 

#include "../../../include/cppmath/abstract_algebra/ring/Ring.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

using namespace cppmath::abstract_algebra::example;

// --- Mod4Addition ---
Mod4Addition::Mod4Addition() : cppmath::abstract_algebra::AlgebraicOperation<IntegerMod4Element, IntegerMod4>("Mod4Addition", 2) {}
IntegerMod4Element Mod4Addition::operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const {
    int sum = (a.int_value() + b.int_value()) % 4;
    return IntegerMod4Element(a.parent_structure(), sum);
}
bool Mod4Addition::is_associative(const IntegerMod4&) const { return true; }
bool Mod4Addition::is_commutative(const IntegerMod4&) const { return true; }

// --- Mod4Multiplication ---
Mod4Multiplication::Mod4Multiplication() : cppmath::abstract_algebra::AlgebraicOperation<IntegerMod4Element, IntegerMod4>("Mod4Multiplication", 2) {}
IntegerMod4Element Mod4Multiplication::operate(const IntegerMod4Element& a, const IntegerMod4Element& b, const IntegerMod4& ring) const {
    int prod = (a.int_value() * b.int_value()) % 4;
    return IntegerMod4Element(a.parent_structure(), prod);
}
bool Mod4Multiplication::is_associative(const IntegerMod4&) const { return true; }
bool Mod4Multiplication::is_commutative(const IntegerMod4&) const { return true; }

// --- IntegerMod4 ---
IntegerMod4::IntegerMod4() : Ring("Z/4Z", 4), add_op_(), mul_op_() {}
const cppmath::abstract_algebra::AlgebraicOperation<IntegerMod4Element, IntegerMod4>& IntegerMod4::addition_op() const { return add_op_; }
const cppmath::abstract_algebra::AlgebraicOperation<IntegerMod4Element, IntegerMod4>& IntegerMod4::multiplication_op() const { return mul_op_; }
IntegerMod4Element IntegerMod4::identity() const { return IntegerMod4Element(std::make_shared<IntegerMod4>(*this), 1); }
bool IntegerMod4::contains(const IntegerMod4Element& element) const {
    int v = element.int_value();
    return v >= 0 && v < 4;
}
std::vector<IntegerMod4Element> IntegerMod4::elements() const {
    std::vector<IntegerMod4Element> elems;
    auto self = std::make_shared<IntegerMod4>(*this);
    for (int i = 0; i < 4; ++i) {
        elems.emplace_back(self, i);
    }
    return elems;
}

// --- IntegerMod4Element ---
IntegerMod4Element::IntegerMod4Element(std::shared_ptr<const IntegerMod4> parent, int value)
    : RingElement<IntegerMod4Element, IntegerMod4>(parent, value % 4) {}
int IntegerMod4Element::int_value() const { return value_; }
IntegerMod4Element IntegerMod4Element::add(const IntegerMod4Element& other, const IntegerMod4& ring) const {
    return ring.addition_op().operate(*this, other, ring);
}
IntegerMod4Element IntegerMod4Element::multiply(const IntegerMod4Element& other, const IntegerMod4& ring) const {
    return ring.multiplication_op().operate(*this, other, ring);
} 