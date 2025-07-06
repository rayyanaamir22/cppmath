#include <iostream>
#include <memory>
#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/FiniteSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"

using namespace cppmath::sets;

int main() {
    // IntervalSet demo
    auto interval1 = IntervalSet::closed(0, 5); // [0, 5]
    auto interval2 = IntervalSet::open(3, 7);   // (3, 7)
    std::cout << "interval1: " << interval1->toString() << std::endl;
    std::cout << "interval2: " << interval2->toString() << std::endl;

    // FiniteSet demo
    FiniteSet finite({1, 2, 3});
    std::cout << "finite: " << finite.toString() << std::endl;
    finite.add_element(4);
    std::cout << "finite after add 4: " << finite.toString() << std::endl;
    finite.remove_element(2);
    std::cout << "finite after remove 2: " << finite.toString() << std::endl;

    // UnionSet demo
    auto union_set = interval1->set_union(finite);
    std::cout << "union: " << union_set->toString() << std::endl;

    // IntersectionSet demo
    auto intersection_set = interval2->intersect(finite);
    std::cout << "intersection: " << intersection_set->toString() << std::endl;

    // EmptySet and UniversalSet
    std::cout << "EmptySet: " << EmptySet::instance().toString() << std::endl;
    std::cout << "UniversalSet: " << UniversalSet::instance().toString() << std::endl;

    // Composite operations
    auto comp = union_set->complement();
    std::cout << "complement of union: " << comp->toString() << std::endl;

    // Membership tests
    std::cout << "interval1 contains 3? " << (interval1->contains(3) ? "yes" : "no") << std::endl;
    std::cout << "finite contains 2? " << (finite.contains(2) ? "yes" : "no") << std::endl;
    std::cout << "finite contains 4? " << (finite.contains(4) ? "yes" : "no") << std::endl;

    return 0;
} 