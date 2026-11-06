#pragma once
#include <string>
#include <utility>

namespace cppmath {
namespace core {

/**
 * @brief The "type" of a term: Real, Bool, Set, ...
 *
 * A Sort is just a name, compared by name. It is deliberately not an enum so
 * that other modules can introduce their own sorts (Matrix, Group, ...)
 * without editing core. `Any` is a wildcard used by operators that accept
 * every sort.
 */
class Sort {
public:
    /// A sort called `name`; sorts with the same name are equal.
    explicit Sort(std::string name) : name_(std::move(name)) {}

    /// The sort's name.
    const std::string& name() const { return name_; }
    bool operator==(const Sort& other) const { return name_ == other.name_; }
    bool operator!=(const Sort& other) const { return !(*this == other); }

    /** @brief True if a slot expecting this sort may be filled by a term of sort `actual`. */
    bool accepts(const Sort& actual) const {
        return *this == Any() || actual == Any() || *this == actual;
    }

    /// Real numbers.
    static Sort Real() { return Sort("Real"); }
    /// Truth values.
    static Sort Bool() { return Sort("Bool"); }
    /// Sets of reals (sets::Set).
    static Sort Set() { return Sort("Set"); }
    /// Wildcard: accepted by, and accepts, every sort.
    static Sort Any() { return Sort("Any"); }

private:
    std::string name_;
};

} // namespace core
} // namespace cppmath
