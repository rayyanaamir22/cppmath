#include "cppmath/core/Bind.hpp"
#include <functional>
#include <stdexcept>

namespace cppmath {
namespace core {

Bind::Bind(const Operator& op, std::string variable, std::vector<ExprPtr> operands)
    : op_(&op), var_(std::move(variable), op.bound_sort()), operands_(std::move(operands)) {
    // var_ is built first, so an invalid identifier already threw std::invalid_argument.
    if (op_->notation() != Notation::Binder)
        throw std::invalid_argument("Bind: operator '" + op_->name() + "' is not a binder");
    if (!op_->accepts_count(operands_.size()))
        throw std::invalid_argument("Bind: binder '" + op_->name() + "' cannot take " +
                                    std::to_string(operands_.size()) + " operand(s) after the variable");
    for (std::size_t i = 0; i < operands_.size(); ++i) {
        if (!operands_[i])
            throw std::invalid_argument("Bind: null operand " + std::to_string(i) + " to '" + op_->name() + "'");
        if (!op_->input_sort(i).accepts(operands_[i]->sort()))
            throw std::invalid_argument("Bind: operand " + std::to_string(i) + " of '" + op_->name() +
                                        "' must have sort " + op_->input_sort(i).name() + ", got " +
                                        operands_[i]->sort().name());
    }
}

ExprPtr Bind::clone() const {
    std::vector<ExprPtr> copies;
    copies.reserve(operands_.size());
    for (const auto& o : operands_) copies.push_back(o->clone());
    return std::make_unique<Bind>(*op_, var_.name(), std::move(copies));
}

bool Bind::equals(const Expression& other) const {
    auto* o = dynamic_cast<const Bind*>(&other);
    // Operators compare by name, as in Apply, so trees from a copied Signature still match.
    if (!o || o->op_->name() != op_->name() || o->var_.name() != var_.name() ||
        o->operands_.size() != operands_.size())
        return false;
    for (std::size_t i = 0; i < operands_.size(); ++i)
        if (!operands_[i]->equals(*o->operands_[i])) return false;
    return true;
}

std::size_t Bind::hash() const {
    std::size_t h = detail::hash_combine(0x4, std::hash<std::string>()(op_->name()));
    h = detail::hash_combine(h, std::hash<std::string>()(var_.name()));
    for (const auto& o : operands_) h = detail::hash_combine(h, o->hash());
    return h;
}

} // namespace core
} // namespace cppmath
