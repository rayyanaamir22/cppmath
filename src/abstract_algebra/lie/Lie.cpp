// Implementation for Lie class
// ... existing code ... 

#include "../../../include/cppmath/abstract_algebra/lie/Lie.hpp"
#include <memory>
#include <vector>
#include <cmath>

using namespace cppmath::abstract_algebra::example;

// --- HeisenbergBracket ---
HeisenbergBracket::HeisenbergBracket() : cppmath::abstract_algebra::AlgebraicOperation<HeisenbergElement, HeisenbergLie>("HeisenbergBracket", 2) {}
HeisenbergElement HeisenbergBracket::operate(const HeisenbergElement& a, const HeisenbergElement& b, const HeisenbergLie& lie) const {
    // [a, b] = (0, 0, a.x * b.y - a.y * b.x)
    double z = a.x() * b.y() - a.y() * b.x();
    return HeisenbergElement(a.parent_structure(), 0.0, 0.0, z);
}
bool HeisenbergBracket::is_associative(const HeisenbergLie&) const { return false; }
bool HeisenbergBracket::is_commutative(const HeisenbergLie&) const { return false; }

// --- HeisenbergLie ---
HeisenbergLie::HeisenbergLie() : Lie("Heisenberg", 0), bracket_() {}
const cppmath::abstract_algebra::AlgebraicOperation<HeisenbergElement, HeisenbergLie>& HeisenbergLie::bracket_op() const { return bracket_; }
HeisenbergElement HeisenbergLie::identity() const { return HeisenbergElement(std::make_shared<HeisenbergLie>(*this), 0.0, 0.0, 0.0); }
bool HeisenbergLie::contains(const HeisenbergElement& element) const { return true; }
std::vector<HeisenbergElement> HeisenbergLie::elements() const {
    std::vector<HeisenbergElement> elems;
    auto self = std::make_shared<HeisenbergLie>(*this);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                elems.emplace_back(self, x, y, z);
            }
        }
    }
    return elems;
}

// --- HeisenbergElement ---
HeisenbergElement::HeisenbergElement(std::shared_ptr<const HeisenbergLie> parent, double x, double y, double z)
    : parent_(parent), x_(x), y_(y), z_(z) {}
double HeisenbergElement::x() const { return x_; }
double HeisenbergElement::y() const { return y_; }
double HeisenbergElement::z() const { return z_; }
HeisenbergElement HeisenbergElement::bracket(const HeisenbergElement& other, const HeisenbergLie& lie) const {
    return lie.bracket_op().operate(*this, other, lie);
}
bool HeisenbergElement::equals(const HeisenbergElement& other) const {
    return std::abs(x_ - other.x_) < 1e-9 && std::abs(y_ - other.y_) < 1e-9 && std::abs(z_ - other.z_) < 1e-9;
} 