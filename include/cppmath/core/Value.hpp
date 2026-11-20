#pragma once
#include "Sort.hpp"
#include "cppmath/sets/Set.hpp"
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace cppmath {
namespace core {

/// A set shared between Values. Sets are immutable, so sharing is safe and copying a Value stays cheap.
using SetPtr = std::shared_ptr<const sets::Set>;

/**
 * @brief A value of one of the sorts an expression can have: a Real, a Bool, or a Set.
 *
 * `Interpretation<Carrier>` has a single carrier type, but an expression such as
 * `x in [0, 1) U {5}` mixes sorts (a Real, a Set, a Bool). `Value` is the carrier for that:
 * a tagged union with checked accessors, so using a value as the wrong sort throws instead of
 * misbehaving. Other modules can follow the same pattern with their own carrier (for example one
 * that adds matrices).
 *
 * Invariant: a Set value is never null.
 */
class Value {
public:
    /// A Real value.
    static Value real(double v);
    /// A Bool value.
    static Value boolean(bool b);
    /** @brief A Set value. @throws std::invalid_argument if `s` is null. */
    static Value set(SetPtr s);
    /** @brief A Set value taking ownership of `s` (any concrete set, e.g. a `unique_ptr<IntervalSet>`). */
    template <class S>
    static Value set(std::unique_ptr<S> s) {
        return set(SetPtr(std::move(s)));
    }

    /// Sort::Real(), Sort::Bool() or Sort::Set().
    Sort sort() const;
    /// Whether this is a Real.
    bool is_real() const { return v_.index() == 0; }
    /// Whether this is a Bool.
    bool is_bool() const { return v_.index() == 1; }
    /// Whether this is a Set.
    bool is_set() const { return v_.index() == 2; }

    /** @brief The Real. @throws std::invalid_argument if this is not a Real. */
    double as_real() const;
    /** @brief The Bool. @throws std::invalid_argument if this is not a Bool. */
    bool as_bool() const;
    /** @brief The Set. @throws std::invalid_argument if this is not a Set. */
    const sets::Set& as_set() const;
    /** @brief The shared Set pointer (never null). @throws std::invalid_argument if this is not a Set. */
    SetPtr shared_set() const;

    /**
     * @brief Equality of values of the *same* sort (false across sorts).
     *
     * Sets compare with `sets::Set::equals`, which is structural: `{1}` and `[1, 1]` are different
     * kinds of set and compare unequal even though they contain the same numbers.
     */
    bool equals(const Value& other) const;

    /// Text for the value: `3`, `true`, `[0, 1)`, `{1, 2}`.
    std::string toString() const;

private:
    explicit Value(std::variant<double, bool, SetPtr> v) : v_(std::move(v)) {}
    std::variant<double, bool, SetPtr> v_;
};

} // namespace core
} // namespace cppmath
