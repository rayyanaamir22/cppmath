#pragma once

namespace cppmath {
namespace core {

class Constant;
class Variable;
class Apply;

/**
 * @brief Double-dispatch interface over the AST node kinds.
 *
 * There are only a few node kinds (operators are data inside Apply, not
 * subclasses), so this interface stays small as the operator set grows.
 * Most algorithms should use `fold` / `transform` instead of subclassing this.
 */
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    /// Called for a numeric literal.
    virtual void visit(const Constant&) = 0;
    /// Called for a variable.
    virtual void visit(const Variable&) = 0;
    /// Called for an operator application (including atoms).
    virtual void visit(const Apply&) = 0;
};

} // namespace core
} // namespace cppmath
