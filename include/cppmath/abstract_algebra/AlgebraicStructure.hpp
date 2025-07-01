#pragma once

namespace cppmath {
namespace abstract_algebra {

template<typename ElementType>
class AlgebraicStructure {
public:
    virtual ~AlgebraicStructure() = default;
    // Abstract method for structure-specific logic (e.g., identity element)
    virtual ElementType identity() const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 