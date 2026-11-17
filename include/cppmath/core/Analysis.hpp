#pragma once
#include "Expression.hpp"
#include <cstddef>
#include <set>
#include <string>

namespace cppmath {
namespace core {

/**
 * @brief Names of the *free* variables of `expr`.
 *
 * A variable bound by a Bind is not free inside that binder's body, but it is free
 * in the binder's bounds, which lie outside its scope: `sum(i, 1, i, i)` has free
 * variable `i` (from the upper bound).
 */
std::set<std::string> free_variables(const Expression& expr);
/** @brief Whether `name` is a free variable of `expr`. */
bool contains_variable(const Expression& expr, const std::string& name);
/** @brief Number of nodes in the tree (a leaf counts 1). */
std::size_t node_count(const Expression& expr);
/** @brief Height of the tree (a leaf has depth 1). */
std::size_t tree_depth(const Expression& expr);

/**
 * @brief Equality up to renaming of bound variables (α-equivalence).
 *
 * `sum(i, 1, 3, i)` and `sum(j, 1, 3, j)` mean the same thing, but `Expression::equals`
 * (which is syntactic) says they differ. This says they match. Free variables must
 * have the same name: `sum(i, 1, 3, i + x)` is not α-equivalent to `sum(i, 1, 3, i + y)`.
 */
bool alpha_equivalent(const Expression& a, const Expression& b);

} // namespace core
} // namespace cppmath
