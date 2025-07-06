#pragma once
#include "Set.hpp"
#include <set>
#include <vector>
#include <initializer_list>
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class FiniteSet : public Set {
public:
    std::set<double> elements;

    FiniteSet(std::initializer_list<double> vals = {});
    FiniteSet(const std::vector<double>& vals);

    bool contains(double value) const override;
    std::unique_ptr<Set> set_union(const Set& other) const override;
    std::unique_ptr<Set> intersect(const Set& other) const override;
    std::unique_ptr<Set> complement() const override;
    std::string toString() const override;
    std::unique_ptr<Set> clone() const override;
    bool equals(const Set& other) const override;

    void add_element(double value);
    void remove_element(double value);
    bool is_empty() const;
    size_t size() const;
};

} // namespace sets
} // namespace cppmath 