#include "cppmath/sets/EmptySet.hpp"
#include "cppmath/sets/UniversalSet.hpp"
#include <memory>

namespace cppmath {
namespace sets {

EmptySet& EmptySet::instance() {
    static EmptySet instance;
    return instance;
}

bool EmptySet::contains(double) const {
    return false;
}

std::unique_ptr<Set> EmptySet::set_union(const Set& other) const {
    return other.clone();
}

std::unique_ptr<Set> EmptySet::intersect(const Set&) const {
    return clone();
}

std::unique_ptr<Set> EmptySet::complement() const {
    return UniversalSet::instance().clone();
}

std::string EmptySet::toString() const {
    return "∅";
}

std::unique_ptr<Set> EmptySet::clone() const {
    return std::make_unique<EmptySet>(*this);
}

bool EmptySet::equals(const Set& other) const {
    return dynamic_cast<const EmptySet*>(&other) != nullptr;
}

} // namespace sets
} // namespace cppmath 