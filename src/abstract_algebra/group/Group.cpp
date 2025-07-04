// Implementation for Group class
// ... existing code ... 

#include "../../../include/cppmath/abstract_algebra/group/Group.hpp"
#include <algorithm>
#include <stdexcept>
#include <memory>

using namespace cppmath::abstract_algebra::example;

// --- PermutationComposition ---
PermutationComposition::PermutationComposition() : cppmath::abstract_algebra::AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3>("PermutationComposition", 2) {}

SymmetricGroup3Element PermutationComposition::operate(const SymmetricGroup3Element& a, const SymmetricGroup3Element& b, const SymmetricGroup3& group) const {
    // Compose two permutations: a(b(x))
    std::vector<int> result(3);
    const auto& p = a.permutation();
    const auto& q = b.permutation();
    for (int i = 0; i < 3; ++i) {
        result[i] = p[q[i]];
    }
    return SymmetricGroup3Element(a.parent_structure(), result);
}

bool PermutationComposition::is_associative(const SymmetricGroup3&) const { return true; }
bool PermutationComposition::is_commutative(const SymmetricGroup3&) const { return false; }

// --- SymmetricGroup3 ---
SymmetricGroup3::SymmetricGroup3() : Group("S_3", 6), op_() {}

const cppmath::abstract_algebra::AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3>& SymmetricGroup3::operation() const { return op_; }
bool SymmetricGroup3::is_abelian() const { return false; }

SymmetricGroup3Element SymmetricGroup3::identity() const {
    return SymmetricGroup3Element(std::make_shared<SymmetricGroup3>(*this), {0, 1, 2});
}

bool SymmetricGroup3::contains(const SymmetricGroup3Element& element) const {
    const auto& perm = element.permutation();
    if (perm.size() != 3) return false;
    std::vector<int> check = perm;
    std::sort(check.begin(), check.end());
    return check == std::vector<int>({0, 1, 2});
}

std::vector<SymmetricGroup3Element> SymmetricGroup3::elements() const {
    std::vector<SymmetricGroup3Element> elems;
    std::vector<int> perm = {0, 1, 2};
    auto self = std::make_shared<SymmetricGroup3>(*this);
    do {
        elems.emplace_back(self, perm);
    } while (std::next_permutation(perm.begin(), perm.end()));
    return elems;
}

// --- SymmetricGroup3Element ---
SymmetricGroup3Element::SymmetricGroup3Element(std::shared_ptr<const SymmetricGroup3> parent, const std::vector<int>& perm)
    : GroupElement(parent, 0), perm_(perm) {}

const std::vector<int>& SymmetricGroup3Element::permutation() const { return perm_; }

SymmetricGroup3Element SymmetricGroup3Element::inverse(const SymmetricGroup3& group) const {
    std::vector<int> inv(3);
    for (int i = 0; i < 3; ++i) {
        inv[perm_[i]] = i;
    }
    return SymmetricGroup3Element(this->parent_structure(), inv);
}

SymmetricGroup3Element SymmetricGroup3Element::identity(const SymmetricGroup3& group) const {
    return SymmetricGroup3Element(this->parent_structure(), {0, 1, 2});
}

SymmetricGroup3Element SymmetricGroup3Element::operate(const SymmetricGroup3Element& other, const SymmetricGroup3& group) const {
    return group.operation().operate(*this, other, group);
}

bool SymmetricGroup3Element::equals(const SymmetricGroup3Element& other) const {
    return perm_ == other.permutation();
} 