#include "cppmath/core/Variable.hpp"
#include <cctype>
#include <functional>
#include <stdexcept>

namespace cppmath {
namespace core {

namespace {
bool is_identifier(const std::string& s) {
    if (s.empty()) return false;
    unsigned char first = static_cast<unsigned char>(s[0]);
    if (!std::isalpha(first) && first != '_') return false;
    for (unsigned char ch : s)
        if (!std::isalnum(ch) && ch != '_') return false;
    return true;
}
} // namespace

Variable::Variable(std::string name, Sort sort) : name_(std::move(name)), sort_(std::move(sort)) {
    if (!is_identifier(name_))
        throw std::invalid_argument("Variable: '" + name_ + "' is not a valid identifier");
}

ExprPtr Variable::clone() const { return std::make_unique<Variable>(name_, sort_); }

bool Variable::equals(const Expression& other) const {
    auto* o = dynamic_cast<const Variable*>(&other);
    return o && o->name_ == name_ && o->sort_ == sort_;
}

std::size_t Variable::hash() const {
    return detail::hash_combine(0x2, std::hash<std::string>()(name_));
}

} // namespace core
} // namespace cppmath
