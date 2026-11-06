#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace cppmath {
namespace core {

/**
 * @brief Variable assignment x -> value in some carrier type, with optional outer scope.
 *
 * A child Environment (made with the parent pointer) sees its parent's
 * bindings but may shadow them; this is what binders will use later.
 * The parent is non-owning and must outlive the child.
 */
template <class Carrier>
class Environment {
public:
    /// A top-level scope with no bindings.
    Environment() = default;
    /// A child scope that falls back to `parent` (non-owning) for names it doesn't bind.
    explicit Environment(const Environment* parent) : parent_(parent) {}

    /// Bind `name` to `value` in this scope, replacing any earlier binding here.
    void bind(const std::string& name, Carrier value) { vars_.insert_or_assign(name, std::move(value)); }

    /// Whether `name` is bound here or in an enclosing scope.
    bool has(const std::string& name) const {
        return vars_.count(name) > 0 || (parent_ && parent_->has(name));
    }

    /** @throws std::out_of_range if `name` is unbound here and in every enclosing scope. */
    const Carrier& lookup(const std::string& name) const {
        auto it = vars_.find(name);
        if (it != vars_.end()) return it->second;
        if (parent_) return parent_->lookup(name);
        throw std::out_of_range("unbound variable '" + name + "'");
    }

private:
    const Environment* parent_ = nullptr;
    std::map<std::string, Carrier> vars_;
};

} // namespace core
} // namespace cppmath
