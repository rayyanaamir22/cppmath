#pragma once
#include "../AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract element of a ring: supports + and *.
 *
 * @tparam Derived       the concrete element class (CRTP).
 * @tparam StructureType the ring class it belongs to.
 *
 * Stores an integer `value_`; `equals()` compares it. The inherited `operate()` means **addition**.
 */
template<typename Derived, typename StructureType>
class RingElement : public AlgebraicElement<Derived, StructureType> {
protected:
    int value_;
public:
    /** @param value integer representative. */
    RingElement(std::shared_ptr<const StructureType> parent_structure = nullptr, int value = 0)
        : AlgebraicElement<Derived, StructureType>(parent_structure), value_(value) {}
    virtual ~RingElement() = default;
    /** @brief `*this + other` in `structure`. */
    virtual Derived add(const Derived& other, const StructureType& structure) const = 0;
    /** @brief `*this * other` in `structure`. */
    virtual Derived multiply(const Derived& other, const StructureType& structure) const = 0;
    virtual int value() const override { return value_; }
    /** @brief Compares integer values. */
    virtual bool equals(const Derived& other) const override { return value_ == other.value(); }
    /** @brief The primary operation, which for a ring is addition: same as `add()`. */
    virtual Derived operate(const Derived& other, const StructureType& structure) const override {
        return this->add(other, structure);
    }
};

} // namespace abstract_algebra
} // namespace cppmath 