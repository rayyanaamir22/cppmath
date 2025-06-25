#pragma once
#include "AlgebraicStructure.hpp"

namespace cppmath {
namespace abstract_algebra {

class Group : public AlgebraicStructure {
public:
    virtual ~Group() = default;
    virtual bool is_abelian() const = 0;
    // Add more group-specific virtual methods as needed
};

} // namespace abstract_algebra
} // namespace cppmath 