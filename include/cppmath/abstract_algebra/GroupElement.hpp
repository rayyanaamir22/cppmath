#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

class GroupElement : public AlgebraicElement {
public:
    virtual ~GroupElement() = default;
    virtual GroupElement* inverse() const = 0;
    virtual GroupElement* identity() const = 0;
    virtual GroupElement* operate(const GroupElement& other) const = 0;
    // Add more group element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 