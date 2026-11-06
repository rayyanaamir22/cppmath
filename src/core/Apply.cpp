#include "cppmath/core/Apply.hpp"
#include <functional>
#include <stdexcept>

namespace cppmath {
namespace core {

Apply::Apply(const Operator& op, std::vector<ExprPtr> args) : op_(&op), args_(std::move(args)) {
    if (!op_->accepts_count(args_.size()))
        throw std::invalid_argument("Apply: operator '" + op_->name() + "' cannot take " +
                                    std::to_string(args_.size()) + " argument(s)");
    for (std::size_t i = 0; i < args_.size(); ++i) {
        if (!args_[i])
            throw std::invalid_argument("Apply: null argument " + std::to_string(i) + " to '" + op_->name() + "'");
        if (!op_->input_sort(i).accepts(args_[i]->sort()))
            throw std::invalid_argument("Apply: argument " + std::to_string(i) + " of '" + op_->name() +
                                        "' must have sort " + op_->input_sort(i).name() + ", got " +
                                        args_[i]->sort().name());
    }
}

ExprPtr Apply::clone() const {
    std::vector<ExprPtr> copies;
    copies.reserve(args_.size());
    for (const auto& a : args_) copies.push_back(a->clone());
    return std::make_unique<Apply>(*op_, std::move(copies));
}

bool Apply::equals(const Expression& other) const {
    auto* o = dynamic_cast<const Apply*>(&other);
    // Operators compare by name so trees built from a copied/extended Signature still match.
    if (!o || o->op_->name() != op_->name() || o->args_.size() != args_.size()) return false;
    for (std::size_t i = 0; i < args_.size(); ++i)
        if (!args_[i]->equals(*o->args_[i])) return false;
    return true;
}

std::size_t Apply::hash() const {
    std::size_t h = detail::hash_combine(0x3, std::hash<std::string>()(op_->name()));
    for (const auto& a : args_) h = detail::hash_combine(h, a->hash());
    return h;
}

} // namespace core
} // namespace cppmath
