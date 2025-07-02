#include "../include/cppmath/abstract_algebra/field/Field.hpp"
#include <iostream>
#include <memory>

using namespace cppmath::abstract_algebra::example;

int main() {
    // Create a finite field of order 7 (mod 7)
    auto field = std::make_shared<FiniteField>(7);
    FiniteFieldElement a(field, 3);
    FiniteFieldElement b(field, 5);

    auto sum = a.add(b, *field);
    auto prod = a.multiply(b, *field);
    auto inv = a.inverse(*field);

    std::cout << "a = " << a.int_value() << std::endl;
    std::cout << "b = " << b.int_value() << std::endl;
    std::cout << "a + b = " << sum.int_value() << std::endl;
    std::cout << "a * b = " << prod.int_value() << std::endl;
    std::cout << "a^-1 = " << inv.int_value() << std::endl;
    return 0;
} 