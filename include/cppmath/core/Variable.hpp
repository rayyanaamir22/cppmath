#pragma once
#include "Expression.hpp"

namespace cppmath {
namespace core {

/**
 * @brief A named unknown, e.g. x.
 *
 * Names are identifiers: a letter or '_' followed by letters, digits, '_'.
 * Note that a name equal to an Atom symbol of the Signature in use (such as
 * "pi" or "e" in the standard one) would be read back as that constant, so
 * `build::var` rejects those.
 */
class Variable : public Expression {
public:
    /** @throws std::invalid_argument if `name` is not a valid identifier. */
    explicit Variable(std::string name, Sort sort = Sort::Real());

    /// The variable's name.
    const std::string& name() const { return name_; }

    Sort sort() const override { return sort_; }
    ExprPtr clone() const override;
    bool equals(const Expression& other) const override;
    std::size_t hash() const override;
    void accept(ExpressionVisitor& v) const override { v.visit(*this); }

private:
    std::string name_;
    Sort sort_;
};

} // namespace core
} // namespace cppmath
