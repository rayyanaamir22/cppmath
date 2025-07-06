#pragma once
#include "Set.hpp"
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class UniversalSet : public Set {
private:
    UniversalSet() = default;
public:
    static UniversalSet& instance();

    bool contains(double value) const override;
    std::unique_ptr<Set> set_union(const Set& other) const override;
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;
    UniversalSet(const UniversalSet&) = default;
};

} // namespace sets
} // namespace cppmath 