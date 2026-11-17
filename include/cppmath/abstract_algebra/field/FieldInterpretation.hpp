#pragma once
#include "cppmath/abstract_algebra/field/Field.hpp"   // first: Field.hpp and FieldElement.hpp include each other
#include "cppmath/core/Interpretation.hpp"
#include <memory>

namespace cppmath {
namespace abstract_algebra {
namespace example {

/**
 * @brief Meaning of the standard core operators in the finite field Z/pZ.
 *
 * With this interpretation, one parsed expression can be evaluated over the reals
 * (`core::evaluate_real`) or in F_p (`core::evaluate`) by changing only the
 * interpretation. For expressions built from `+ - * /` and numerals the two agree
 * after reduction mod p, because evaluation is a ring homomorphism.
 *
 *  - **Defined:** `add`, `sub`, `mul`, `div`, `neg`.
 *  - **Numerals** must be integer-valued (3, not 0.5) and are reduced mod p.
 *  - **Left undefined**, so evaluation throws `std::out_of_range`: `pow`, `sin`, `cos`,
 *    `tan`, `exp`, `log`, `sqrt`, `abs`, `pi`, `e`. `pow` is left out on purpose: its
 *    exponent is itself a field element, so `x^7` in F_7 would read the exponent as 0
 *    (giving 1) where Fermat's little theorem says x. Write `x*x` instead.
 *
 * Evaluating with this interpretation can throw:
 *  - `std::invalid_argument` for division by 0, or a numeral that is not an integer.
 *
 * @param field the field to compute in; shared with every element the interpretation creates.
 * @throws std::invalid_argument if `field` is null.
 *
 * @code
 *   auto f7 = std::make_shared<FiniteField>(7);
 *   auto interp = finite_field_interpretation(f7);
 *   core::Environment<FiniteFieldElement> env;
 *   env.bind("x", FiniteFieldElement(f7, 3));
 *   auto r = core::evaluate(*core::parse::Parser().parse("x*x + 3*x + 1"), interp, env);  // 19 mod 7 = 5
 * @endcode
 */
core::Interpretation<FiniteFieldElement> finite_field_interpretation(std::shared_ptr<const FiniteField> field);

} // namespace example
} // namespace abstract_algebra
} // namespace cppmath
