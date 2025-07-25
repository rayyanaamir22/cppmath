#include "cppmath/core/Constant.hpp"
#include <sstream>

namespace cppmath {
namespace core {

Constant::Constant(double v) : value(v) {}
std::string Constant::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
std::unique_ptr<Expression> Constant::clone() const { return std::make_unique<Constant>(*this); }
double Constant::evaluate() const { return value; }

} // namespace core
} // namespace cppmath 