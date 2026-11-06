#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Fold.hpp"
#include <algorithm>

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
        });
}

} // namespace core
} // namespace cppmath
