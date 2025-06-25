#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

class RingElement : public AlgebraicElement {
public:
    virtual ~RingElement() = default;
    virtual RingElement* add(const RingElement& other) const = 0;
    virtual RingElement* multiply(const RingElement& other) const = 0;
    // Add more ring element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 