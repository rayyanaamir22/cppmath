#pragma once
#include "AlgebraicStructure.hpp"

namespace cppmath {
namespace abstract_algebra {

class VectorSpace : public AlgebraicStructure {
public:
    virtual ~VectorSpace() = default;
    // Add vector space-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 