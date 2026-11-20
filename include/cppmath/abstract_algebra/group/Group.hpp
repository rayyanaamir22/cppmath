#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "GroupElement.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract group (G, *): one associative operation with an identity and inverses.
 *
 * @tparam Derived     the concrete group class (CRTP), e.g. SymmetricGroup3.
 * @tparam ElementType its element class, e.g. SymmetricGroup3Element.
 *
 * Implement `operation()`, `is_abelian()`, and the AlgebraicStructure members
 * (`identity()`, `contains()`, `elements()`).
 */
template<typename Derived, typename ElementType>
class Group : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Group() = default;
    /** @brief The group operation *. */
    virtual const AlgebraicOperation<ElementType, Derived>& operation() const = 0;

    /** @brief The identity element: the identity of `operation()`. */
    ElementType identity() const override {
        return operation().identity(static_cast<const Derived&>(*this));
    }
    /** @brief Whether the group is abelian (its operation is commutative). */
    virtual bool is_abelian() const = 0;
};

/** @brief Example concrete structures for tests and demos; not part of the abstract hierarchy. */
namespace example {

class SymmetricGroup3;
class SymmetricGroup3Element;
class PermutationComposition;

/**
 * @brief Composition of permutations of {0, 1, 2}: `operate(a, b)` is a(b(x)), i.e. apply b first.
 *
 * Associative, not commutative; the identity is the permutation {0, 1, 2}.
 */
class PermutationComposition : public AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3> {
public:
    /// The binary operation, named "PermutationComposition".
    PermutationComposition();
    bool has_identity(const SymmetricGroup3& group) const override;
    SymmetricGroup3Element identity(const SymmetricGroup3& group) const override;
    /** @brief a(b(x)): composes the permutations, applying b first. */
    SymmetricGroup3Element operate(const SymmetricGroup3Element& a, const SymmetricGroup3Element& b, const SymmetricGroup3& group) const override;
    bool is_associative(const SymmetricGroup3& group) const override;
    bool is_commutative(const SymmetricGroup3& group) const override;
};

/**
 * @brief Example: the symmetric group S_3 of all 6 permutations of {0, 1, 2} under composition.
 *
 * Non-abelian. `identity()` and `elements()` return elements whose parent is a
 * fresh copy of this group.
 */
class SymmetricGroup3 : public Group<SymmetricGroup3, SymmetricGroup3Element> {
    PermutationComposition op_;
public:
    /// The group S_3, named "S_3" (order 6).
    SymmetricGroup3();
    const AlgebraicOperation<SymmetricGroup3Element, SymmetricGroup3>& operation() const override;
    bool is_abelian() const override;
    /** @brief True if the element's permutation is a rearrangement of {0, 1, 2}. */
    bool contains(const SymmetricGroup3Element& element) const override;
    /** @brief All 6 permutations, in lexicographic order. */
    std::vector<SymmetricGroup3Element> elements() const override;
};

/**
 * @brief Example: a permutation of {0, 1, 2}, stored as images: perm[i] is where i goes.
 *
 * Equality compares the permutation vectors.
 */
class SymmetricGroup3Element : public GroupElement<SymmetricGroup3Element, SymmetricGroup3> {
    std::vector<int> perm_;
public:
    /** @param perm images of 0, 1, 2; not validated here (use SymmetricGroup3::contains). */
    SymmetricGroup3Element(std::shared_ptr<const SymmetricGroup3> parent, const std::vector<int>& perm);
    /// The image vector: permutation()[i] is the image of i.
    const std::vector<int>& permutation() const;
    /** @brief The inverse permutation, so that p * p^{-1} is the identity. */
    SymmetricGroup3Element inverse(const SymmetricGroup3& group) const override;
    /** @brief The identity permutation {0, 1, 2}. */
    SymmetricGroup3Element identity(const SymmetricGroup3& group) const override;
    /** @brief Composition `*this * other`, i.e. apply other first, via the group's operation. */
    SymmetricGroup3Element operate(const SymmetricGroup3Element& other, const SymmetricGroup3& group) const override;
    bool equals(const SymmetricGroup3Element& other) const override;
    int value() const override { return 0; } // Not meaningful for permutations, but required by interface
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 