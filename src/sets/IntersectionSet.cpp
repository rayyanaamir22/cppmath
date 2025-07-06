#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include <sstream>

namespace cppmath {
namespace sets {

IntersectionSet::IntersectionSet(std::unique_ptr<Set> s1, std::unique_ptr<Set> s2)
    : set1(std::move(s1)), set2(std::move(s2)) {}

bool IntersectionSet::contains(double value) const {
    return set1->contains(value) && set2->contains(value);
}

std::unique_ptr<Set> IntersectionSet::set_union(const Set& other) const {
    // using thm: (A ∩ B) U C = (A U C) ∩ (B U C)
    return std::make_unique<IntersectionSet>(set1->set_union(other), set2->set_union(other));
}

std::unique_ptr<Set> IntersectionSet::intersect(const Set& other) const {
    // using thm: (A ∩ B) ∩ C = A ∩ (B ∩ C)
    return std::make_unique<IntersectionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> IntersectionSet::complement() const {
    // using thm: (A ∩ B)^c = A^c U B^c
    return std::make_unique<UnionSet>(set1->complement(), set2->complement());
}

std::string IntersectionSet::toString() const {
    std::ostringstream oss;
    oss << "(" << set1->toString() << " ∩ " << set2->toString() << ")";
    return oss.str();
}

std::unique_ptr<Set> IntersectionSet::clone() const {
    return std::make_unique<IntersectionSet>(set1->clone(), set2->clone());
}

bool IntersectionSet::equals(const Set& other) const {
    auto o = dynamic_cast<const IntersectionSet*>(&other);
    if (!o) return false;
    // using thm: (A ∩ B) == (A ∩ B) or (B ∩ A)
    return (set1->equals(*o->set1) && set2->equals(*o->set2)) ||
           (set1->equals(*o->set2) && set2->equals(*o->set1));
}

} // namespace sets
} // namespace cppmath 