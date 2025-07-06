#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include <sstream>

namespace cppmath {
namespace sets {

UnionSet::UnionSet(std::unique_ptr<Set> s1, std::unique_ptr<Set> s2)
    : set1(std::move(s1)), set2(std::move(s2)) {}

bool UnionSet::contains(double value) const {
    return set1->contains(value) || set2->contains(value);
}

std::unique_ptr<Set> UnionSet::set_union(const Set& other) const {
    // (A U B) U C -> A U (B U C)
    return std::make_unique<UnionSet>(this->clone(), other.clone());
}

std::unique_ptr<Set> UnionSet::intersect(const Set& other) const {
    // (A U B) ∩ C = (A ∩ C) U (B ∩ C)
    return std::make_unique<UnionSet>(set1->intersect(other), set2->intersect(other));
}

std::unique_ptr<Set> UnionSet::complement() const {
    // (A U B)^c = A^c ∩ B^c
    return std::make_unique<IntersectionSet>(set1->complement(), set2->complement());
}

std::string UnionSet::toString() const {
    std::ostringstream oss;
    oss << "(" << set1->toString() << " U " << set2->toString() << ")";
    return oss.str();
}

std::unique_ptr<Set> UnionSet::clone() const {
    return std::make_unique<UnionSet>(set1->clone(), set2->clone());
}

bool UnionSet::equals(const Set& other) const {
    auto o = dynamic_cast<const UnionSet*>(&other);
    if (!o) return false;
    // (A U B) == (A U B) or (B U A)
    return (set1->equals(*o->set1) && set2->equals(*o->set2)) ||
           (set1->equals(*o->set2) && set2->equals(*o->set1));
}

} // namespace sets
} // namespace cppmath 