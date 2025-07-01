#pragma once
#include <string>
#include <vector>

namespace cppmath {
namespace abstract_algebra {

template<typename ElementType>
class AlgebraicStructure {
protected:
    std::string name_;
    size_t order_;
public:
    AlgebraicStructure(const std::string& name = "", size_t order = 0) : name_(name), order_(order) {}
    virtual ~AlgebraicStructure() = default;
    // Get the name of the structure
    virtual std::string name() const { return name_; }
    // Get the order (number of elements) of the structure
    virtual size_t order() const { return order_; }
    // Abstract: identity element
    virtual ElementType identity() const = 0;
    // Abstract: check if an element belongs to the structure
    virtual bool contains(const ElementType& element) const = 0;
    // Abstract: get all elements (if finite)
    virtual std::vector<ElementType> elements() const = 0;
};

} // namespace abstract_algebra
} // namespace cppmath 