#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

class VectorElement : public AlgebraicElement {
public:
    virtual ~VectorElement() = default;
    virtual VectorElement* add(const VectorElement& other) const = 0;
    virtual VectorElement* scalar_multiply(double scalar) const = 0;
    // Add more vector element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 