#include "../include/cppmath/abstract_algebra/lie/Lie.hpp"
#include <iostream>
#include <memory>

using namespace cppmath::abstract_algebra::example;

void print_elem(const HeisenbergElement& e) {
    std::cout << "(" << e.x() << ", " << e.y() << ", " << e.z() << ")";
}

int main() {
    auto lie = std::make_shared<HeisenbergLie>();
    HeisenbergElement a(lie, 1, 0, 0);
    HeisenbergElement b(lie, 0, 1, 0);
    HeisenbergElement c(lie, 0, 0, 1);

    std::cout << "a = "; print_elem(a); std::cout << ", b = "; print_elem(b); std::cout << ", c = "; print_elem(c); std::cout << std::endl;

    auto ab = a.bracket(b, *lie);
    std::cout << "[a, b] = "; print_elem(ab); std::cout << std::endl;
    auto ba = b.bracket(a, *lie);
    std::cout << "[b, a] = "; print_elem(ba); std::cout << std::endl;
    std::cout << "Antisymmetry: [a, b] + [b, a] = ";
    print_elem(HeisenbergElement(lie, ab.x() + ba.x(), ab.y() + ba.y(), ab.z() + ba.z()));
    std::cout << std::endl;

    // Jacobi identity: [a, [b, c]] + [b, [c, a]] + [c, [a, b]] = 0 (identity element)
    auto bc = b.bracket(c, *lie);
    auto ca = c.bracket(a, *lie);
    auto ab2 = a.bracket(b, *lie);
    auto jacobi = a.bracket(bc, *lie);
    jacobi = HeisenbergElement(lie, jacobi.x() + b.bracket(ca, *lie).x() + c.bracket(ab2, *lie).x(),
                                    jacobi.y() + b.bracket(ca, *lie).y() + c.bracket(ab2, *lie).y(),
                                    jacobi.z() + b.bracket(ca, *lie).z() + c.bracket(ab2, *lie).z());
    std::cout << "Jacobi: [a, [b, c]] + [b, [c, a]] + [c, [a, b]] = "; print_elem(jacobi); std::cout << std::endl;
    return 0;
} 