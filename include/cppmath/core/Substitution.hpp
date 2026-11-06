#pragma once
#include "Expression.hpp"
#include <map>
#include <string>

namespace cppmath {
namespace core {

/**
 * @brief A finite map from variable names to expressions, applied simultaneously.
 *
 * Applying {x -> y, y -> x} to (x + y) gives (y + x): replacements are not
 * themselves rewritten again. (Capture-avoidance only matters once binders
 * exist; see PLAN.md milestone 6.)
 */
class Substitution {
public:
    /** @brief Bind `name` to a copy of `replacement`, replacing any earlier binding. */
    void bind(const std::string& name, const Expression& replacement);
    /// Whether `name` has a replacement.
    bool binds(const std::string& name) const { return map_.count(name) > 0; }
    /// Number of bound variables.
    std::size_t size() const { return map_.size(); }

    /** @brief New tree with every bound variable replaced.
     *  @throws std::invalid_argument if a replacement's sort doesn't fit the variable it replaces. */
    ExprPtr apply(const Expression& expr) const;

private:
    friend Substitution compose(const Substitution&, const Substitution&);
    std::map<std::string, ExprPtr> map_;
};

/**
 * @brief Composite substitution: apply(compose(s, t), e) == t.apply(s.apply(e)).
 *
 * That is, `first` happens before `second`. (Kleisli composition for the term monad.)
 */
Substitution compose(const Substitution& first, const Substitution& second);

} // namespace core
} // namespace cppmath
