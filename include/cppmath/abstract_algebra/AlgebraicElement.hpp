#pragma once
#include <memory>

namespace cppmath {
namespace abstract_algebra {

/**
 * @brief Root of every element type: an element that knows the structure it belongs to.
 *
 * CRTP: `Derived` is the concrete element class itself, so methods can take and
 * return `Derived` by value with no casts.
 *
 * @tparam Derived       the concrete element type (e.g. FiniteFieldElement).
 * @tparam StructureType the structure it lives in (e.g. FiniteField).
 *
 * The parent is shared, not owned exclusively. The example structures hand out
 * elements whose parent is a *copy* of the structure (`make_shared(*this)`), not the original.
 */
template<typename Derived, typename StructureType>
class AlgebraicElement {
protected:
    std::shared_ptr<const StructureType> parent_structure_;
public:
    /** @param parent_structure the structure this element belongs to; may be null. */
    AlgebraicElement(std::shared_ptr<const StructureType> parent_structure = nullptr)
        : parent_structure_(parent_structure) {}
    virtual ~AlgebraicElement() = default;
    /** @brief The structure this element belongs to (may be null). */
    virtual std::shared_ptr<const StructureType> parent_structure() const { return parent_structure_; }
    /** @brief Element equality, as defined by the concrete type. */
    virtual bool equals(const Derived& other) const = 0;
    /**
     * @brief Integer representative of the element.
     *
     * Meaningful only for integer-backed elements (Z/nZ, finite fields). Types
     * without one (permutations, vectors, Lie elements) return 0.
     */
    virtual int value() const = 0;
    /** @brief The structure's primary operation: `*this op other` in `structure`. */
    virtual Derived operate(const Derived& other, const StructureType& structure) const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 