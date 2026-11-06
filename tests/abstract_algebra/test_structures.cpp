// Tests for the example structures (S3, Z/4Z, F_p, R^2, R^n, Heisenberg) and the
// per-operation identity design: each AlgebraicOperation knows its own identity element.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <memory>
#include <stdexcept>
#include "cppmath/abstract_algebra/field/Field.hpp"   // first: Field.hpp and FieldElement.hpp include each other
#include "cppmath/abstract_algebra/group/Group.hpp"
#include "cppmath/abstract_algebra/lie/Lie.hpp"
#include "cppmath/abstract_algebra/ring/Ring.hpp"
#include "cppmath/abstract_algebra/vector/VectorSpace.hpp"

using namespace cppmath::abstract_algebra::example;

TEST_CASE("Group: identity() is the identity of the group operation") {
    SymmetricGroup3 g;
    CHECK(g.operation().has_identity(g));
    CHECK(g.identity().permutation() == std::vector<int>{0, 1, 2});
    CHECK(g.operation().identity(g).equals(g.identity()));

    auto e = g.identity();
    for (const auto& a : g.elements()) {
        CHECK(a.operate(e, g).equals(a));
        CHECK(e.operate(a, g).equals(a));
        CHECK(a.operate(a.inverse(g), g).equals(e));
    }
    CHECK_FALSE(g.is_abelian());
}

TEST_CASE("Ring: identity() is 0 (addition), one() is 1 (multiplication)") {
    IntegerMod4 r;
    CHECK(r.identity().int_value() == 0);
    CHECK(r.one().int_value() == 1);
    CHECK(r.addition_op().has_identity(r));
    CHECK(r.multiplication_op().has_identity(r));
    CHECK(r.addition_op().identity(r).int_value() == 0);
    CHECK(r.multiplication_op().identity(r).int_value() == 1);

    auto self = std::make_shared<IntegerMod4>();
    for (const auto& a : r.elements()) {
        CHECK(a.add(r.identity(), *self).equals(a));
        CHECK(a.multiply(r.one(), *self).equals(a));
        CHECK(a.multiply(r.identity(), *self).int_value() == 0);   // 0 absorbs
    }
}

TEST_CASE("Ring: values are reduced into 0..3, including negatives") {
    auto r = std::make_shared<IntegerMod4>();
    CHECK(IntegerMod4Element(r, 5).int_value() == 1);
    CHECK(IntegerMod4Element(r, -1).int_value() == 3);
    CHECK(IntegerMod4Element(r, -4).int_value() == 0);
    CHECK(r->contains(IntegerMod4Element(r, -7)));   // 1
}

TEST_CASE("Field: identity() is 0, one() is 1") {
    FiniteField f(7);
    CHECK(f.identity().int_value() == 0);
    CHECK(f.one().int_value() == 1);
    CHECK(f.multiplication_op().identity(f).int_value() == 1);
}

TEST_CASE("FiniteField only accepts prime moduli") {
    for (int p : {2, 3, 5, 7, 11, 13, 97}) CHECK_NOTHROW(FiniteField{p});
    for (int p : {-7, 0, 1, 4, 6, 9, 15, 91}) CHECK_THROWS_AS(FiniteField{p}, std::invalid_argument);
}

TEST_CASE("FiniteFieldElement: reduction, null parent, inverses") {
    auto f = std::make_shared<FiniteField>(7);
    CHECK(FiniteFieldElement(f, 10).int_value() == 3);
    CHECK(FiniteFieldElement(f, -1).int_value() == 6);
    CHECK(FiniteFieldElement(f, -8).int_value() == 6);
    CHECK(f->contains(FiniteFieldElement(f, -3)));

    CHECK_THROWS_AS(FiniteFieldElement(nullptr, 1), std::invalid_argument);

    for (int v = 1; v < 7; ++v) {
        FiniteFieldElement a(f, v);
        CHECK(a.multiply(a.inverse(*f), *f).int_value() == 1);
    }
    CHECK_THROWS_AS(FiniteFieldElement(f, 0).inverse(*f), std::invalid_argument);
}

TEST_CASE("VectorSpace: identity() is the zero vector") {
    R2VectorSpace r2;
    CHECK(r2.identity().x() == 0.0);
    CHECK(r2.identity().y() == 0.0);
    CHECK(r2.addition_op().has_identity(r2));

    auto self = std::make_shared<R2VectorSpace>();
    R2Vector v(self, 3, -2);
    CHECK(v.add(r2.identity(), r2).equals(v));

    NumericVectorSpace rn(4);
    auto z = rn.identity();
    REQUIRE(z.data().size() == 4);
    for (double c : z.data()) CHECK(c == 0.0);
    CHECK(rn.contains(z));
}

TEST_CASE("NumericAddition rejects vectors of different lengths") {
    auto rn = std::make_shared<NumericVectorSpace>(3);
    NumericVectorElement a(rn, {1, 2, 3});
    NumericVectorElement b(rn, {1, 2});
    CHECK_THROWS_AS(a.add(b, *rn), std::invalid_argument);
    CHECK_THROWS_AS(b.add(a, *rn), std::invalid_argument);
    CHECK(a.add(a, *rn).data() == std::vector<double>{2, 4, 6});
}

TEST_CASE("NumericScalarMultiplication rejects an empty scalar vector") {
    auto rn = std::make_shared<NumericVectorSpace>(3);
    NumericVectorElement v(rn, {1, 2, 3});
    NumericVectorElement empty(rn, std::vector<double>{});
    CHECK_THROWS_AS(rn->scalar_multiplication_op().operate(v, empty, *rn), std::invalid_argument);
    NumericVectorElement two(rn, {2});
    CHECK(rn->scalar_multiplication_op().operate(v, two, *rn).data() == std::vector<double>{2, 4, 6});
}

TEST_CASE("Lie: the bracket has no identity element") {
    HeisenbergLie h;
    CHECK_FALSE(h.bracket_op().has_identity(h));
    CHECK_THROWS_AS(h.bracket_op().identity(h), std::logic_error);
    CHECK(h.identity().x() == 0.0);   // the zero vector is still the structure's identity()

    auto self = std::make_shared<HeisenbergLie>();
    HeisenbergElement X(self, 1, 0, 0), Y(self, 0, 1, 0);
    CHECK(X.bracket(Y, h).z() == doctest::Approx(1.0));
    CHECK(Y.bracket(X, h).z() == doctest::Approx(-1.0));
}
