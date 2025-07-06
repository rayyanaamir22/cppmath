#pragma once
#include <memory>
#include <string>

namespace cppmath {
namespace sets {

class Set {
public:
    virtual ~Set() = default;
    virtual bool contains(double value) const = 0;
    virtual std::unique_ptr<Set> set_union(const Set& other) const = 0;
    virtual std::unique_ptr<Set> intersect(const Set& other) const = 0;
    virtual std::unique_ptr<Set> complement() const = 0;
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<Set> clone() const = 0;
    virtual bool equals(const Set& other) const = 0;
};

} // namespace sets
} // namespace cppmath 