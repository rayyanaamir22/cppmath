#pragma once
#include "AlgebraicElement.hpp"

namespace cppmath {
namespace abstract_algebra {

class FieldElement : public AlgebraicElement {
public:
    virtual ~FieldElement() = default;
    virtual FieldElement* add(const FieldElement& other) const = 0;
    virtual FieldElement* multiply(const FieldElement& other) const = 0;
    virtual FieldElement* inverse() const = 0;
    // Add more field element-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 