#include "cppmath/core/Substitution.hpp"
#include "cppmath/core/Fold.hpp"
#include <stdexcept>

namespace cppmath {
namespace core {

void Substitution::bind(const std::string& name, const Expression& replacement) {
    map_[name] = replacement.clone();
}

ExprPtr Substitution::apply(const Expression& expr) const {
    // transform() visits only the original nodes, so a replacement is inserted
    // as-is and never re-substituted: the substitution is simultaneous.
    return transform(expr, [&](ExprPtr node) -> ExprPtr {
        auto* var = dynamic_cast<const Variable*>(node.get());
        if (!var) return node;
        auto it = map_.find(var->name());
        if (it == map_.end()) return node;
        if (!var->sort().accepts(it->second->sort()))
            throw std::invalid_argument("Substitution: cannot replace " + var->name() + " : " +
                                        var->sort().name() + " by a term of sort " +
                                        it->second->sort().name());
        return it->second->clone();
    });
}

Substitution compose(const Substitution& first, const Substitution& second) {
    Substitution out;
    for (const auto& kv : first.map_) out.map_[kv.first] = second.apply(*kv.second);
    for (const auto& kv : second.map_)
        if (!first.binds(kv.first)) out.map_[kv.first] = kv.second->clone();
    return out;
}

} // namespace core
} // namespace cppmath
