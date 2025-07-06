#include "cppmath/sets/UniversalSet.hpp"
#include "cppmath/sets/EmptySet.hpp"
#include <memory>

namespace cppmath {
namespace sets {

UniversalSet& UniversalSet::instance() {
    static UniversalSet instance;
    return instance;
}

bool UniversalSet::contains(double) const {
    return true;
}

std::unique_ptr<Set> UniversalSet::set_union(const Set&) const {
    return clone();
}

std::unique_ptr<Set> UniversalSet::intersect(const Set& other) const {
    return other.clone();
}

std::unique_ptr<Set> UniversalSet::complement() const {
    return EmptySet::instance().clone();
}

std::string UniversalSet::toString() const {
    return "ℝ";
}

std::unique_ptr<Set> UniversalSet::clone() const {
    return std::make_unique<UniversalSet>(*this);
}

bool UniversalSet::equals(const Set& other) const {
    return dynamic_cast<const UniversalSet*>(&other) != nullptr;
}

} // namespace sets
} // namespace cppmath 