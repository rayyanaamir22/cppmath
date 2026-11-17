#include "cppmath/abstract_algebra/field/FieldInterpretation.hpp"
#include <cmath>
#include <stdexcept>
#include <vector>

namespace cppmath {
namespace abstract_algebra {
namespace example {

core::Interpretation<FiniteFieldElement> finite_field_interpretation(std::shared_ptr<const FiniteField> field) {
    if (!field) throw std::invalid_argument("finite_field_interpretation: field must not be null");
    using E = FiniteFieldElement;
    using Args = std::vector<E>;

    // A numeral n means "n times the multiplicative identity", so it must be an integer.
    // fmod is exact on doubles, so large numerals reduce correctly without overflowing int.
    core::Interpretation<E> interp([field](double v) {
        if (v != std::floor(v))
            throw std::invalid_argument("F_p: numeral " + std::to_string(v) + " is not an integer");
        return E(field, static_cast<int>(std::fmod(v, static_cast<double>(field->modulus()))));
    });

    // Every operation goes through the element methods, which use the field's own operations.
    auto negate = [field](const E& a) { return E(field, -a.int_value()); };

    interp.define("add", [field](const Args& a) { return a[0].add(a[1], *field); });
    interp.define("sub", [field, negate](const Args& a) { return a[0].add(negate(a[1]), *field); });
    interp.define("mul", [field](const Args& a) { return a[0].multiply(a[1], *field); });
    // inverse() throws std::invalid_argument for 0, matching division by zero over the reals.
    interp.define("div", [field](const Args& a) { return a[0].multiply(a[1].inverse(*field), *field); });
    interp.define("neg", [negate](const Args& a) { return negate(a[0]); });
    return interp;
}

} // namespace example
} // namespace abstract_algebra
} // namespace cppmath
