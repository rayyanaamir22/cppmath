#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "GroupElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract Group structure: Derived is the concrete group, ElementType is the type of elements in the group
// Inherit and implement for concrete groups

template<typename Derived, typename ElementType>
class Group : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Group() = default;
    // Get the group operation
    virtual const AlgebraicOperation<ElementType, Derived>& operation() const = 0;
    // Check if the group is abelian (commutative)
    virtual bool is_abelian() const = 0;
    // Add more group-specific virtual methods as needed
};

namespace example {

class SymmetricGroup3;
class SymmetricGroup3Element;
class PermutationComposition;

class PermutationComposition : public AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3> {
public:
    PermutationComposition();
    SymmetricGroup3Element operate(const SymmetricGroup3Element& a, const SymmetricGroup3Element& b, const SymmetricGroup3& group) const;
    bool is_associative(const SymmetricGroup3& group) const;
    bool is_commutative(const SymmetricGroup3& group) const;
};

class SymmetricGroup3 : public Group<SymmetricGroup3, SymmetricGroup3Element> {
    PermutationComposition op_;
public:
    SymmetricGroup3();
    const AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3>& operation() const;
    bool is_abelian() const;
    SymmetricGroup3Element identity() const;
    bool contains(const SymmetricGroup3Element& element) const;
    std::vector<SymmetricGroup3Element> elements() const;
};

class SymmetricGroup3Element : public GroupElement<SymmetricGroup3Element, SymmetricGroup3> {
    std::vector<int> perm_;
public:
    SymmetricGroup3Element(std::shared_ptr<const SymmetricGroup3> parent, const std::vector<int>& perm);
    const std::vector<int>& permutation() const;
    SymmetricGroup3Element inverse(const SymmetricGroup3& group) const;
    SymmetricGroup3Element identity(const SymmetricGroup3& group) const;
    SymmetricGroup3Element operate(const SymmetricGroup3Element& other, const SymmetricGroup3& group) const;
    bool equals(const SymmetricGroup3Element& other) const;
    int value() const override { return 0; } // Not meaningful for permutations, but required by interface
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 