#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstdlib>
#include <limits>
#include <string>
#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/FiniteSet.hpp"
#include "cppmath/sets/IntersectionSet.hpp"
#include "cppmath/sets/IntervalSet.hpp"
#include "cppmath/sets/UnionSet.hpp"
#include "cppmath/sets/UniversalSet.hpp"

using namespace cppmath::sets;

namespace {
const double INF = std::numeric_limits<double>::infinity();
// IntervalSet(lower, upper, lower_included, upper_included)
std::unique_ptr<IntervalSet> iv(double a, double b, bool ai, bool bi) {
    return std::make_unique<IntervalSet>(a, b, ai, bi);
}
}

TEST_CASE("IntervalSet: membership and text") {
    auto s = IntervalSet::closed_open(0, 1);
    CHECK(s->contains(0));
    CHECK(s->contains(0.5));
    CHECK_FALSE(s->contains(1));
    CHECK_FALSE(s->contains(-0.1));
    CHECK(s->toString() == "[0, 1)");
    CHECK(IntervalSet::positive_real()->toString() == "(0, inf)");
    CHECK(IntervalSet::negative_real()->toString() == "(-inf, 0)");
}

TEST_CASE("IntervalSet::set_union: overlapping intervals merge") {
    auto u = IntervalSet::closed(0, 2)->set_union(*IntervalSet::closed(1, 3));
    CHECK(u->toString() == "[0, 3]");
    CHECK(IntervalSet::closed(0, 5)->set_union(*IntervalSet::closed(1, 2))->toString() == "[0, 5]");  // containment
}

TEST_CASE("IntervalSet::set_union: touching ends merge only if the shared point is included") {
    CHECK(IntervalSet::closed(0, 1)->set_union(*IntervalSet::closed(1, 2))->toString() == "[0, 2]");
    CHECK(IntervalSet::closed(0, 1)->set_union(*IntervalSet::open(1, 2))->toString() == "[0, 2)");   // 1 is in the first
    CHECK(IntervalSet::open(0, 1)->set_union(*IntervalSet::closed(1, 2))->toString() == "(0, 2]");   // 1 is in the second

    // 1 belongs to neither, so the result must NOT contain it
    auto gap = IntervalSet::open(0, 1)->set_union(*IntervalSet::open(1, 2));
    CHECK(dynamic_cast<UnionSet*>(gap.get()) != nullptr);
    CHECK_FALSE(gap->contains(1));
    CHECK(gap->contains(0.5));
    CHECK(gap->contains(1.5));
}

TEST_CASE("IntervalSet::set_union: separated intervals stay a UnionSet") {
    auto u = IntervalSet::closed(0, 1)->set_union(*IntervalSet::closed(2, 3));
    CHECK(u->toString() == "([0, 1] U [2, 3])");
    CHECK_FALSE(u->contains(1.5));
}

TEST_CASE("IntervalSet::set_union: a shared endpoint is included if either interval includes it") {
    CHECK(iv(0, 1, false, false)->set_union(*iv(0, 2, true, false))->toString() == "[0, 2)");   // was "(0, 2)"
    CHECK(iv(0, 2, true, false)->set_union(*iv(0, 1, false, false))->toString() == "[0, 2)");   // order doesn't matter
    CHECK(iv(0, 1, false, true)->set_union(*iv(-1, 1, false, false))->toString() == "(-1, 1]");
}

TEST_CASE("IntervalSet::intersect: shared endpoint is included only if both include it") {
    CHECK(iv(0, 1, true, true)->intersect(*iv(0, 1, false, true))->toString() == "(0, 1]");   // was "[0, 1]"
    CHECK(iv(0, 1, false, true)->intersect(*iv(0, 1, true, true))->toString() == "(0, 1]");
    CHECK(iv(0, 1, true, true)->intersect(*iv(0, 1, true, false))->toString() == "[0, 1)");
    CHECK(IntervalSet::closed(0, 2)->intersect(*IntervalSet::open(1, 3))->toString() == "(1, 2]");
}

TEST_CASE("IntervalSet::intersect: empty cases") {
    CHECK(IntervalSet::closed(0, 1)->intersect(*IntervalSet::closed(2, 3))->equals(EmptySet::instance()));
    // [0,1] and (1,2] meet at 1 but only the first contains it
    CHECK(IntervalSet::closed(0, 1)->intersect(*iv(1, 2, false, true))->equals(EmptySet::instance()));
    // [0,1] and [1,2] share exactly the point 1
    auto point = IntervalSet::closed(0, 1)->intersect(*IntervalSet::closed(1, 2));
    CHECK(point->contains(1));
    CHECK_FALSE(point->contains(0.5));
}

TEST_CASE("IntervalSet::complement") {
    CHECK(IntervalSet::closed(0, 1)->complement()->toString() == "((-inf, 0) U (1, inf))");
    CHECK(IntervalSet::positive_real()->complement()->toString() == "(-inf, 0]");
    CHECK(IntervalSet::all_real()->complement()->equals(EmptySet::instance()));
}

TEST_CASE("IntervalSet::equals works for infinite bounds") {
    CHECK(IntervalSet::all_real()->equals(*IntervalSet::all_real()));   // inf - inf is NaN: used to be false
    CHECK(IntervalSet::positive_real()->equals(*IntervalSet::positive_real()));
    CHECK_FALSE(IntervalSet::positive_real()->equals(*IntervalSet::non_negative_real()));
    CHECK_FALSE(IntervalSet::all_real()->equals(UniversalSet::instance()));   // structural, not set-theoretic
}

TEST_CASE("FiniteSet: basics") {
    FiniteSet s({3, 1, 2, 2});
    CHECK(s.size() == 3);
    CHECK(s.toString() == "{1, 2, 3}");
    CHECK(s.contains(2));
    CHECK(s.contains(2 + 1e-12));    // tolerance
    CHECK_FALSE(s.contains(4));
    CHECK(FiniteSet().is_empty());
}

TEST_CASE("FiniteSet::with_element / without_element return new sets and leave the original alone") {
    FiniteSet s({1, 2, 3});
    auto a = s.with_element(4);
    auto b = s.without_element(2);
    CHECK(a->toString() == "{1, 2, 3, 4}");
    CHECK(b->toString() == "{1, 3}");
    CHECK(s.toString() == "{1, 2, 3}");

    CHECK(s.with_element(2)->equals(s));              // already present
    CHECK(s.with_element(2 + 1e-12)->size() == 3);    // within tolerance of 2
    CHECK(s.without_element(9)->equals(s));           // absent
    CHECK(s.without_element(2 - 1e-12)->toString() == "{1, 3}");
}

TEST_CASE("FiniteSet union and intersection use the same tolerance as contains") {
    FiniteSet a({1.0, 2.0});
    FiniteSet b({2.0 + 1e-12, 3.0});
    auto u = a.set_union(b);
    CHECK(u->toString() == "{1, 2, 3}");               // 2 and 2+1e-12 are one element
    auto i = a.intersect(b);
    CHECK(i->toString() == "{2}");                     // used to be empty: exact comparison
    CHECK(i->contains(2.0));
}

TEST_CASE("FiniteSet::complement") {
    CHECK(FiniteSet({1, 2}).complement()->toString() == "(((-inf, 1) U (1, 2)) U (2, inf))");
    CHECK(FiniteSet().complement()->equals(UniversalSet::instance()));
}

TEST_CASE("toString keeps full precision (so it can be parsed back)") {
    CHECK(IntervalSet::closed(0.1, 0.2)->toString() == "[0.1, 0.2]");
    CHECK(IntervalSet::closed(1.0 / 3.0, 1)->toString() == "[0.3333333333333333, 1]");
    CHECK(FiniteSet({0.1, 1.0 / 3.0}).toString() == "{0.1, 0.3333333333333333}");
    CHECK(IntervalSet::closed(0.123456789, 5)->toString() == "[0.123456789, 5]");   // not cut to 6 digits

    // whatever the digit count, the text must parse back to exactly the same double
    for (double v : {1.0 / 3.0, 0.1 + 0.2, 1e-7, 123456789.123456789, 2.0 / 7.0}) {
        std::string text = FiniteSet({v}).toString();           // "{<number>}"
        CHECK(std::strtod(text.substr(1, text.size() - 2).c_str(), nullptr) == v);
    }
}

TEST_CASE("Union / Intersection / Empty / Universal") {
    auto u = std::make_unique<UnionSet>(IntervalSet::closed(0, 1), std::make_unique<FiniteSet>(std::initializer_list<double>{5}));
    CHECK(u->contains(0.5));
    CHECK(u->contains(5));
    CHECK_FALSE(u->contains(3));
    CHECK(u->toString() == "([0, 1] U {5})");

    auto x = std::make_unique<IntersectionSet>(IntervalSet::closed(0, 10), IntervalSet::closed(5, 20));
    CHECK(x->contains(7));
    CHECK_FALSE(x->contains(3));

    CHECK_FALSE(EmptySet::instance().contains(0));
    CHECK(UniversalSet::instance().contains(1e300));
    CHECK(EmptySet::instance().complement()->equals(UniversalSet::instance()));
    CHECK(EmptySet::instance().set_union(*IntervalSet::closed(0, 1))->equals(*IntervalSet::closed(0, 1)));
    CHECK(UniversalSet::instance().intersect(*IntervalSet::closed(0, 1))->equals(*IntervalSet::closed(0, 1)));
}
