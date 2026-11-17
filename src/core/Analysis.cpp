#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Fold.hpp"
#include <algorithm>
#include <utility>

namespace cppmath {
namespace core {

std::set<std::string> free_variables(const Expression& expr) {
    return fold<std::set<std::string>>(
        expr,
        [](const Constant&) { return std::set<std::string>(); },
        [](const Variable& x) { return std::set<std::string>{x.name()}; },
        [](const Operator&, std::vector<std::set<std::string>> kids) {
            std::set<std::string> all;
            for (auto& k : kids) all.insert(k.begin(), k.end());
            return all;
        },
        [](const Bind& b, std::vector<std::set<std::string>> kids) {
            // The bounds are outside the binder's scope; only the body (the last operand) is inside.
            std::set<std::string> all;
            for (std::size_t i = 0; i + 1 < kids.size(); ++i) all.insert(kids[i].begin(), kids[i].end());
            kids.back().erase(b.variable().name());
            all.insert(kids.back().begin(), kids.back().end());
            return all;
        });
}

bool contains_variable(const Expression& expr, const std::string& name) {
    return free_variables(expr).count(name) > 0;
}

std::size_t node_count(const Expression& expr) {
    return fold<std::size_t>(
        expr,
        [](const Constant&) { return std::size_t(1); },
        [](const Variable&) { return std::size_t(1); },
        [](const Operator&, std::vector<std::size_t> kids) {
            std::size_t n = 1;
            for (auto k : kids) n += k;
            return n;
        },
        [](const Bind&, std::vector<std::size_t> kids) {
            std::size_t n = 1;  // the bound variable's name is not a node of its own
            for (auto k : kids) n += k;
            return n;
        });
}

std::size_t tree_depth(const Expression& expr) {
    return fold<std::size_t>(
        expr,
        [](const Constant&) { return std::size_t(1); },
        [](const Variable&) { return std::size_t(1); },
        [](const Operator&, std::vector<std::size_t> kids) {
            std::size_t deepest = 0;
            for (auto k : kids) deepest = std::max(deepest, k);
            return deepest + 1;
        },
        [](const Bind&, std::vector<std::size_t> kids) {
            std::size_t deepest = 0;
            for (auto k : kids) deepest = std::max(deepest, k);
            return deepest + 1;
        });
}

namespace {
// Pairs (name in a, name in b) of the binders we are inside, outermost first.
using Scope = std::vector<std::pair<std::string, std::string>>;

// Index of the innermost binder in `scope` that binds `name` (on side `second` or not), or -1 if free.
int binder_index(const Scope& scope, const std::string& name, bool second) {
    for (int i = static_cast<int>(scope.size()) - 1; i >= 0; --i)
        if ((second ? scope[i].second : scope[i].first) == name) return i;
    return -1;
}

bool alpha_eq(const Expression& a, const Expression& b, Scope& scope) {
    if (auto* ka = dynamic_cast<const Constant*>(&a)) {
        auto* kb = dynamic_cast<const Constant*>(&b);
        return kb && ka->value() == kb->value();
    }
    if (auto* va = dynamic_cast<const Variable*>(&a)) {
        auto* vb = dynamic_cast<const Variable*>(&b);
        if (!vb || va->sort() != vb->sort()) return false;
        // Two variables match if they are bound by the *same* binder, or both free with the same name.
        int ia = binder_index(scope, va->name(), false), ib = binder_index(scope, vb->name(), true);
        return ia == ib && (ia >= 0 || va->name() == vb->name());
    }
    if (auto* pa = dynamic_cast<const Apply*>(&a)) {
        auto* pb = dynamic_cast<const Apply*>(&b);
        if (!pb || pa->op().name() != pb->op().name() || pa->args().size() != pb->args().size()) return false;
        for (std::size_t i = 0; i < pa->args().size(); ++i)
            if (!alpha_eq(*pa->args()[i], *pb->args()[i], scope)) return false;
        return true;
    }
    auto* ba = dynamic_cast<const Bind*>(&a);
    auto* bb = dynamic_cast<const Bind*>(&b);
    if (!ba || !bb || ba->op().name() != bb->op().name() || ba->operands().size() != bb->operands().size())
        return false;
    std::size_t last = ba->operands().size() - 1;
    for (std::size_t i = 0; i < last; ++i)   // bounds: outside the new binder's scope
        if (!alpha_eq(*ba->operands()[i], *bb->operands()[i], scope)) return false;
    scope.emplace_back(ba->variable().name(), bb->variable().name());
    bool body_equal = alpha_eq(ba->body(), bb->body(), scope);
    scope.pop_back();
    return body_equal;
}
} // namespace

bool alpha_equivalent(const Expression& a, const Expression& b) {
    Scope scope;
    return alpha_eq(a, b, scope);
}

} // namespace core
} // namespace cppmath
