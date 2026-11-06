#pragma once
#include "LieElement.hpp"
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract Lie algebra: a vector space with a bilinear, alternating bracket [x, y] obeying the Jacobi identity.
 *
 * @tparam Derived     the concrete Lie algebra class (CRTP), e.g. HeisenbergLie.
 * @tparam ElementType its element class, e.g. HeisenbergElement.
 *
 * Only the bracket is modelled so far; the vector-space operations are not.
 */
template<typename Derived, typename ElementType>
class Lie : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Lie() = default;
    /** @brief The Lie bracket [., .]. */
    virtual const AlgebraicOperation<ElementType, Derived>& bracket_op() const = 0;
};

/** @brief Example concrete structures for tests and demos; not part of the abstract hierarchy. */
namespace example {

class HeisenbergLie;
class HeisenbergElement;
class HeisenbergBracket;

/**
 * @brief Bracket of the 3-dimensional Heisenberg algebra: [a, b] = (0, 0, a.x*b.y - a.y*b.x).
 *
 * The z direction is central, so only x and y contribute. Neither associative nor commutative.
 */
class HeisenbergBracket : public AlgebraicOperation<HeisenbergElement, HeisenbergLie> {
public:
    /// The binary bracket operation, named "HeisenbergBracket".
    HeisenbergBracket();
    /** @brief [a, b] = (0, 0, a.x*b.y - a.y*b.x); the result's parent is a's parent. */
    HeisenbergElement operate(const HeisenbergElement& a, const HeisenbergElement& b, const HeisenbergLie& lie) const override;
    bool is_associative(const HeisenbergLie& lie) const override;
    bool is_commutative(const HeisenbergLie& lie) const override;
};

/**
 * @brief Example: the Heisenberg Lie algebra on R^3, with basis X, Y, Z and [X, Y] = Z.
 *
 * Infinite, so `order()` is 0 and `contains()` accepts every element.
 */
class HeisenbergLie : public Lie<HeisenbergLie, HeisenbergElement> {
    HeisenbergBracket bracket_;
public:
    /// The Heisenberg algebra, named "Heisenberg" (order 0 = infinite).
    HeisenbergLie();
    /** @brief The Heisenberg bracket. */
    const AlgebraicOperation<HeisenbergElement, HeisenbergLie>& bracket_op() const override;
    /** @brief The zero vector (0, 0, 0). */
    HeisenbergElement identity() const override;
    /** @brief Always true: every (x, y, z) is in the algebra. */
    bool contains(const HeisenbergElement& element) const override;
    /** @brief Not all elements (there are infinitely many): the 27 points with x, y, z in {-1, 0, 1}. */
    std::vector<HeisenbergElement> elements() const override; // For demo, return a small set
};

/**
 * @brief Example: the element xX + yY + zZ of the Heisenberg algebra.
 *
 * Keeps its own parent pointer, which `parent_structure()` returns; the base
 * class's `parent_structure_` member stays null.
 */
class HeisenbergElement : public LieElement<HeisenbergElement, HeisenbergLie> {
    std::shared_ptr<const HeisenbergLie> parent_;
    double x_, y_, z_;
public:
    /**
     * @param parent the algebra this element belongs to.
     * @param x coordinate along X.
     * @param y coordinate along Y.
     * @param z coordinate along Z.
     */
    HeisenbergElement(std::shared_ptr<const HeisenbergLie> parent, double x, double y, double z);
    /// The algebra this element was created with.
    std::shared_ptr<const HeisenbergLie> parent_structure() const override { return parent_; }
    /// Coordinate along X.
    double x() const;
    /// Coordinate along Y.
    double y() const;
    /// Coordinate along Z.
    double z() const;
    /** @brief [*this, other], via the algebra's bracket operation. */
    HeisenbergElement bracket(const HeisenbergElement& other, const HeisenbergLie& lie) const override;
    /** @brief Coordinate-wise equality within 1e-9. */
    bool equals(const HeisenbergElement& other) const override;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 