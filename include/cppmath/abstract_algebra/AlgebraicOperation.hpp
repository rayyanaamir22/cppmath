#pragma once

namespace cppmath {
namespace abstract_algebra {

template<typename ElementType, typename StructureType>
class AlgebraicOperation {
public:
    virtual ~AlgebraicOperation() = default;
    // Abstract operation: e.g., binary operation for group, field, etc.
    virtual ElementType operate(const ElementType& a, const ElementType& b, const StructureType& structure) const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 