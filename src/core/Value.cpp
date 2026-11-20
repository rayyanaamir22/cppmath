#include "cppmath/core/Value.hpp"
#include "cppmath/utils/format.hpp"
#include <stdexcept>

namespace cppmath {
namespace core {

Value Value::real(double v) { return Value(std::variant<double, bool, SetPtr>(std::in_place_index<0>, v)); }
Value Value::boolean(bool b) { return Value(std::variant<double, bool, SetPtr>(std::in_place_index<1>, b)); }
Value Value::set(SetPtr s) {
    if (!s) throw std::invalid_argument("Value::set: the set must not be null");
    return Value(std::variant<double, bool, SetPtr>(std::in_place_index<2>, std::move(s)));
}

Sort Value::sort() const {
    switch (v_.index()) {
        case 0: return Sort::Real();
        case 1: return Sort::Bool();
        default: return Sort::Set();
    }
}

namespace {
[[noreturn]] void wrong_sort(const char* wanted, const Value& got) {
    throw std::invalid_argument(std::string("expected a ") + wanted + ", got a " + got.sort().name() + " (" +
                                got.toString() + ")");
}
} // namespace

double Value::as_real() const {
    if (!is_real()) wrong_sort("Real", *this);
    return std::get<0>(v_);
}

bool Value::as_bool() const {
    if (!is_bool()) wrong_sort("Bool", *this);
    return std::get<1>(v_);
}

const sets::Set& Value::as_set() const {
    if (!is_set()) wrong_sort("Set", *this);
    return *std::get<2>(v_);
}

SetPtr Value::shared_set() const {
    if (!is_set()) wrong_sort("Set", *this);
    return std::get<2>(v_);
}

bool Value::equals(const Value& other) const {
    if (v_.index() != other.v_.index()) return false;
    switch (v_.index()) {
        case 0: return std::get<0>(v_) == std::get<0>(other.v_);
        case 1: return std::get<1>(v_) == std::get<1>(other.v_);
        default: return std::get<2>(v_)->equals(*std::get<2>(other.v_));
    }
}

std::string Value::toString() const {
    switch (v_.index()) {
        case 0: return utils::format_number(std::get<0>(v_));
        case 1: return std::get<1>(v_) ? "true" : "false";
        default: return std::get<2>(v_)->toString();
    }
}

} // namespace core
} // namespace cppmath
