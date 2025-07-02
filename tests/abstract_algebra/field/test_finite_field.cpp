#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest/doctest.h"
#include "../../../include/cppmath/abstract_algebra/field/Field.hpp"
#include <memory>
#include <set>

using namespace cppmath::abstract_algebra::example;

TEST_CASE("FiniteField basic properties") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    REQUIRE(elems.size() == 7);
    std::set<int> seen;
    for (const auto& e : elems) {
        seen.insert(e.int_value());
    }
    for (int i = 0; i < 7; ++i) {
        CHECK(seen.count(i) == 1);
    }
}

TEST_CASE("Addition and multiplication closure") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    for (const auto& a : elems) {
        for (const auto& b : elems) {
            auto sum = a.add(b, *field);
            auto prod = a.multiply(b, *field);
            CHECK(field->contains(sum));
            CHECK(field->contains(prod));
        }
    }
}

TEST_CASE("Additive and multiplicative identity") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    auto zero = FiniteFieldElement(field, 0);
    auto one = FiniteFieldElement(field, 1);
    for (const auto& a : elems) {
        CHECK(a.add(zero, *field).int_value() == a.int_value());
        CHECK(a.multiply(one, *field).int_value() == a.int_value());
    }
}

TEST_CASE("Additive inverses") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    auto zero = FiniteFieldElement(field, 0);
    for (const auto& a : elems) {
        bool found = false;
        for (const auto& b : elems) {
            if (a.add(b, *field).int_value() == 0) found = true;
        }
        CHECK(found);
    }
}

TEST_CASE("Multiplicative inverses (except zero)") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    for (const auto& a : elems) {
        if (a.int_value() == 0) continue;
        bool found = false;
        for (const auto& b : elems) {
            if (a.multiply(b, *field).int_value() == 1) found = true;
        }
        CHECK(found);
    }
}

TEST_CASE("Addition and multiplication commutativity") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    for (const auto& a : elems) {
        for (const auto& b : elems) {
            CHECK(a.add(b, *field).int_value() == b.add(a, *field).int_value());
            CHECK(a.multiply(b, *field).int_value() == b.multiply(a, *field).int_value());
        }
    }
}

TEST_CASE("Addition and multiplication associativity") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    for (const auto& a : elems) {
        for (const auto& b : elems) {
            for (const auto& c : elems) {
                int ab_c = a.add(b, *field).add(c, *field).int_value();
                int a_bc = a.add(b.add(c, *field), *field).int_value();
                CHECK(ab_c == a_bc);
                if (a.int_value() != 0 && b.int_value() != 0 && c.int_value() != 0) {
                    int ab_c_m = a.multiply(b, *field).multiply(c, *field).int_value();
                    int a_bc_m = a.multiply(b.multiply(c, *field), *field).int_value();
                    CHECK(ab_c_m == a_bc_m);
                }
            }
        }
    }
}

TEST_CASE("Distributivity") {
    auto field = std::make_shared<FiniteField>(7);
    auto elems = field->elements();
    for (const auto& a : elems) {
        for (const auto& b : elems) {
            for (const auto& c : elems) {
                int left = a.multiply(b.add(c, *field), *field).int_value();
                int right = a.multiply(b, *field).add(a.multiply(c, *field), *field).int_value();
                CHECK(left == right);
            }
        }
    }
} 