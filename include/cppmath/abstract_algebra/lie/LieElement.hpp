#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

class LieElement : public AlgebraicElement {
public:
    virtual ~LieElement() = default;
    virtual LieElement* bracket(const LieElement& other) const = 0;
    // Add more Lie element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 