#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class EmptySet : public Set {
private:
    EmptySet() = default;
public:
    EmptySet(const EmptySet&) = default;
    static EmptySet& instance();

    bool contains(double value) const override;
    std::unique_ptr<Set> set_union(const Set& other) const override;
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
};

} // namespace sets
} // namespace cppmath 