#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class UnionSet : public Set {
public:
    std::unique_ptr<Set> set1;
    std::unique_ptr<Set> set2;

    UnionSet(std::unique_ptr<Set> s1, std::unique_ptr<Set> s2);

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