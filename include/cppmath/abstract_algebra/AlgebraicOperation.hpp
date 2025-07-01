#pragma once
#include <string>

namespace cppmath {
namespace abstract_algebra {

template<typename ElementType, typename StructureType>
class AlgebraicOperation {
protected:
    std::string op_name_;
    size_t arity_;
public:
    AlgebraicOperation(const std::string& op_name = "", size_t arity = 2) : op_name_(op_name), arity_(arity) {}
    virtual ~AlgebraicOperation() = default;
    // Get the name of the operation
    virtual std::string name() const { return op_name_; }
    // Get the arity of the operation
    virtual size_t arity() const { return arity_; }
    // Abstract: perform the operation
    virtual ElementType operate(const ElementType& a, const ElementType& b, const StructureType& structure) const = 0;
    // Abstract: check if operation is associative
    virtual bool is_associative(const StructureType& structure) const = 0;
    // Abstract: check if operation is commutative
    virtual bool is_commutative(const StructureType& structure) const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 