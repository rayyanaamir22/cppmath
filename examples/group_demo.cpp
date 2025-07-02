#include "../include/cppmath/abstract_algebra/group/Group.hpp"
#include <iostream>
#include <memory>

using namespace cppmath::abstract_algebra::example;

void print_perm(const std::vector<int>& perm) {
    // Print a permutation as a list of numbers in square brackets
    std::cout << "[";
    for (size_t i = 0; i < perm.size(); ++i) {
        std::cout << perm[i];
        if (i + 1 < perm.size()) std::cout << ", ";
    }
    std::cout << "]";
}

int main() {
    auto group = std::make_shared<SymmetricGroup3>();
    auto elems = group->elements();
    std::cout << "Elements of S_3:" << std::endl;
    for (const auto& e : elems) {
        print_perm(e.permutation());
        std::cout << std::endl;
    }
    std::cout << "\nIdentity: ";
    print_perm(group->identity().permutation());
    std::cout << std::endl;

    auto a = elems[1]; // (0 1 2) -> (0 2 1)
    auto b = elems[2]; // (0 1 2) -> (1 0 2)
    std::cout << "\nExample: a = "; print_perm(a.permutation());
    std::cout << ", b = "; print_perm(b.permutation()); std::cout << std::endl;
    auto ab = a.operate(b, *group);
    std::cout << "a * b = "; print_perm(ab.permutation()); std::cout << std::endl;
    auto ba = b.operate(a, *group);
    std::cout << "b * a = "; print_perm(ba.permutation()); std::cout << std::endl;
    std::cout << "a * b == b * a? " << (ab.equals(ba) ? "yes" : "no") << std::endl;
    auto ainv = a.inverse(*group);
    std::cout << "Inverse of a: "; print_perm(ainv.permutation()); std::cout << std::endl;
    auto aainv = a.operate(ainv, *group);
    std::cout << "a * a^-1: "; print_perm(aainv.permutation()); std::cout << std::endl;
    return 0;
} 