#pragma once
#include <string>
#include <vector>

namespace cppmath {
namespace abstract_algebra {

/**
 * @brief Root of every algebraic structure: a carrier set of elements with a name and an order.
 *
 * Concrete structures (Group, Ring, Field, VectorSpace, Lie) add their operations on top.
 * Each is a CRTP template `<Derived, ElementType>` that inherits this class.
 *
 * @tparam ElementType the type of the structure's elements.
 */
template<typename ElementType>
class AlgebraicStructure {
protected:
    std::string name_;
    size_t order_;
public:
    /**
     * @param name  human-readable name, e.g. "S_3" or "Z/4Z".
     * @param order number of elements; 0 means infinite or not tracked (e.g. R^2).
     */
    AlgebraicStructure(const std::string& name = "", size_t order = 0) : name_(name), order_(order) {}
    virtual ~AlgebraicStructure() = default;
    /** @brief Human-readable name given at construction. */
    virtual std::string name() const { return name_; }
    /** @brief Number of elements, or 0 if the structure is infinite / the order is not tracked. */
    virtual size_t order() const { return order_; }
    /**
     * @brief The structure's distinguished identity element.
     *
     * For one-operation structures this is the identity of that operation. Rings and
     * fields have two identities; check each concrete type for which one it returns.
     */
    virtual ElementType identity() const = 0;
    /** @brief Membership test: whether `element` belongs to this structure's carrier set. */
    virtual bool contains(const ElementType& element) const = 0;
    /**
     * @brief All elements, for finite structures.
     *
     * Infinite structures may return a small representative sample instead (as the
     * example R^2 and Heisenberg algebra do); see the concrete type.
     */
    virtual std::vector<ElementType> elements() const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 