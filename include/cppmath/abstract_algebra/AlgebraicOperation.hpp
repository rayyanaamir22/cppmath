#pragma once
#include <stdexcept>
#include <string>

namespace cppmath {
namespace abstract_algebra {

/**
 * @brief A named operation on the elements of a structure, e.g. addition mod p.
 *
 * Structures expose their operations through pure virtual accessors
 * (`Group::operation()`, `Ring::addition_op()`, ...), so the same structure
 * class can be paired with different operation objects.
 *
 * @tparam ElementType   type of the operands and result.
 * @tparam StructureType the structure the operation belongs to; passed to every
 *                       call so the operation can read structure data (e.g. the modulus).
 */
template<typename ElementType, typename StructureType>
class AlgebraicOperation {
protected:
    std::string op_name_;
    size_t arity_;
public:
    /**
     * @param op_name human-readable name, e.g. "Addition".
     * @param arity   number of operands (only binary operations exist so far).
     */
    AlgebraicOperation(const std::string& op_name = "", size_t arity = 2) : op_name_(op_name), arity_(arity) {}
    virtual ~AlgebraicOperation() = default;
    /** @brief Human-readable name given at construction. */
    virtual std::string name() const { return op_name_; }
    /** @brief Number of operands; `operate()` itself is always binary. */
    virtual size_t arity() const { return arity_; }
    /** @brief Apply the operation: returns `a op b` computed in `structure`. */
    virtual ElementType operate(const ElementType& a, const ElementType& b, const StructureType& structure) const = 0;
    /** @brief Whether (a op b) op c == a op (b op c) for all elements of `structure`. */
    virtual bool is_associative(const StructureType& structure) const = 0;
    /** @brief Whether a op b == b op a for all elements of `structure`. */
    virtual bool is_commutative(const StructureType& structure) const = 0;

    /**
     * @brief Whether this operation has an identity element e (a op e == a) in `structure`.
     *
     * Defaults to false. Operations such as the Lie bracket have no identity.
     * Structures with several operations get one identity per operation, which is
     * how a ring has both 0 (for +) and 1 (for *).
     */
    virtual bool has_identity(const StructureType&) const { return false; }

    /**
     * @brief The identity element of this operation in `structure`.
     * @throws std::logic_error if the operation has no identity (`has_identity()` is false).
     */
    virtual ElementType identity(const StructureType&) const {
        throw std::logic_error("operation '" + op_name_ + "' has no identity element");
    }
};

// Note: Operations like Addition and Multiplication should inherit from AlgebraicOperation for use in fields.

} // namespace abstract_algebra
} // namespace cppmath 