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
 * themselves rewritten again.
 *
 * Only **free** occurrences are replaced, and binders are respected:
 *  - *Shadowing:* in `sum(i, 1, 3, i + x)` a substitution for `i` leaves the body
 *    alone (that `i` is bound), while `x` is replaced.
 *  - *Capture avoidance:* substituting `x := i` into `sum(i, 1, 3, i + x)` must not
 *    let the new `i` be captured by the binder. The binder's variable is renamed
 *    (`i_1`, `i_2`, ... avoiding every name in sight) and the result is
 *    `sum(i_1, 1, 3, (i_1 + i))`. A binder is renamed only when capture would happen.
 */
class Substitution {
public:
    /** @brief Bind `name` to a copy of `replacement`, replacing any earlier binding. */
    void bind(const std::string& name, const Expression& replacement);
    /// Whether `name` has a replacement.
    bool binds(const std::string& name) const { return map_.count(name) > 0; }
    /// Number of bound variables.
    std::size_t size() const { return map_.size(); }

    /** @brief New tree with every free occurrence of a bound name replaced (see the class notes on binders).
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
 *
 * The equality is exact for expressions without binders. With binders it holds up to
 * renaming of bound variables (`alpha_equivalent`), because capture avoidance may pick a
 * different fresh name along the two routes.
 */
Substitution compose(const Substitution& first, const Substitution& second);

} // namespace core
} // namespace cppmath
