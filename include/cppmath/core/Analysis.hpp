#pragma once
#include "Expression.hpp"
#include <cstddef>
#include <set>
#include <string>

namespace cppmath {
namespace core {

/** @brief Names of all variables occurring in `expr`. */
std::set<std::string> free_variables(const Expression& expr);
/** @brief Whether the variable called `name` occurs in `expr`. */
bool contains_variable(const Expression& expr, const std::string& name);
/** @brief Number of nodes in the tree (a leaf counts 1). */
std::size_t node_count(const Expression& expr);
/** @brief Height of the tree (a leaf has depth 1). */
std::size_t tree_depth(const Expression& expr);

} // namespace core
} // namespace cppmath
