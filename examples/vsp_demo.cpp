#include "../include/cppmath/abstract_algebra/vector/VectorSpace.hpp"
#include <iostream>
#include <memory>
#include <vector>

using namespace cppmath::abstract_algebra::example;

void print_vec(const R2Vector& v) {
    std::cout << "(" << v.x() << ", " << v.y() << ")";
}
void print_numvec(const NumericVectorElement& v) {
    std::cout << "[";
    const auto& d = v.data();
    for (size_t i = 0; i < d.size(); ++i) {
        std::cout << d[i];
        if (i + 1 < d.size()) std::cout << ", ";
    }
    std::cout << "]";
}

int main() {
    std::cout << "--- Abstract R2VectorSpace Demo ---\n";
    auto vspace = std::make_shared<R2VectorSpace>();
    auto elems = vspace->elements();
    std::cout << "Elements of R^2 (demo set):" << std::endl;
    for (const auto& v : elems) {
        print_vec(v);
        std::cout << " ";
    }
    std::cout << std::endl;
    auto a = elems[1]; // (0, -1)
    auto b = elems[4]; // (0, 0)
    std::cout << "\nExample: a = "; print_vec(a);
    std::cout << ", b = "; print_vec(b); std::cout << std::endl;
    auto sum = a.add(b, *vspace);
    std::cout << "a + b = "; print_vec(sum); std::cout << std::endl;
    auto scaled = a.scalar_multiply(2.5, *vspace);
    std::cout << "2.5 * a = "; print_vec(scaled); std::cout << std::endl;
    auto zero = vspace->identity();
    std::cout << "Zero vector: "; print_vec(zero); std::cout << std::endl;

    std::cout << "\n--- Optimized NumericVectorSpace Demo ---\n";
    auto numspace = std::make_shared<NumericVectorSpace>(3);
    auto numelems = numspace->elements();
    std::cout << "Basis elements of R^3:" << std::endl;
    for (const auto& v : numelems) {
        print_numvec(v);
        std::cout << " ";
    }
    std::cout << std::endl;
    NumericVectorElement v1(numspace, {1.0, 2.0, 3.0});
    NumericVectorElement v2(numspace, {4.0, 5.0, 6.0});
    std::cout << "\nv1 = "; print_numvec(v1);
    std::cout << ", v2 = "; print_numvec(v2); std::cout << std::endl;
    auto nsum = v1.add(v2, *numspace);
    std::cout << "v1 + v2 = "; print_numvec(nsum); std::cout << std::endl;
    auto nscaled = v1.scalar_multiply(2.0, *numspace);
    std::cout << "2 * v1 = "; print_numvec(nscaled); std::cout << std::endl;
    auto nzero = numspace->identity();
    std::cout << "Zero vector: "; print_numvec(nzero); std::cout << std::endl;
    return 0;
} 