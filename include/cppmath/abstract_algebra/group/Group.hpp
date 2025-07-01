#pragma once
#include "AlgebraicStructure.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract Group structure: ElementType is the type of elements in the group
// Inherit and implement for concrete groups

template<typename ElementType>
class Group : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~Group() = default;
    // Check if the group is abelian (commutative)
    virtual bool is_abelian() const = 0;
    // Add more group-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 