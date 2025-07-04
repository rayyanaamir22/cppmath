#include "../include/cppmath/abstract_algebra/ring/Ring.hpp"
#include <iostream>
#include <memory>

using namespace cppmath::abstract_algebra::example;

int main() {
    auto ring = std::make_shared<IntegerMod4>();
    auto elems = ring->elements();
    std::cout << "Elements of Z/4Z:" << std::endl;
    for (const auto& e : elems) {
        std::cout << e.int_value() << " ";
    }
    std::cout << std::endl;

    auto a = elems[1]; // 1
    auto b = elems[3]; // 3
    std::cout << "\nExample: a = " << a.int_value() << ", b = " << b.int_value() << std::endl;
    auto sum = a.add(b, *ring);
    auto prod = a.multiply(b, *ring);
    std::cout << "a + b = " << sum.int_value() << std::endl;
    std::cout << "a * b = " << prod.int_value() << std::endl;
    auto one = IntegerMod4Element(ring, 1);
    std::cout << "Identity: " << one.int_value() << std::endl;
    return 0;
} 