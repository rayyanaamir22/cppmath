#include "cppmath/core/Constant.hpp"
#include <cmath>
#include <functional>
#include <stdexcept>

namespace cppmath {
namespace core {

Constant::Constant(double v) : value_(v) {
    if (!std::isfinite(v) || std::signbit(v))
        throw std::invalid_argument("Constant: literals must be finite and non-negative (use build::num for negatives)");
}

ExprPtr Constant::clone() const { return std::make_unique<Constant>(value_); }

bool Constant::equals(const Expression& other) const {
    auto* o = dynamic_cast<const Constant*>(&other);
    return o && o->value_ == value_;
}

std::size_t Constant::hash() const {
    return detail::hash_combine(0x1, std::hash<double>()(value_));
}

} // namespace core
} // namespace cppmath
