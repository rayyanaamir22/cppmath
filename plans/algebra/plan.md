# algebra plan

_Last updated 2026-11-20 · status: **stub** · scope below is *proposed*_

Concrete polynomials: a `Polynomial<T>` type with arithmetic, evaluation, calculus and root finding. It is the computational counterpart of [abstract_algebra](../abstract_algebra/plan.md)'s planned `PolynomialRing`, which uses these polynomials as its elements. It also gives [core](../core/plan.md) a normal form for collecting like terms.

No architecture doc yet. Create `docs/architecture/algebra.md` once `Polynomial<T>` exists.

## Status

- `include/cppmath/algebra.hpp` and `src/algebra/polynomial.cpp` are empty (comments only). No tests, no Makefile rule, no users.
- The header sits at `include/cppmath/algebra.hpp`, not in an `algebra/` folder, unlike every other module.

## Roadmap *(proposed)*

- [ ] **1. Layout.** Move the header to `include/cppmath/algebra/polynomial.hpp` so it mirrors `src/algebra/polynomial.cpp`, and delete `algebra.hpp` (it has no users). Add `tests/algebra/test_polynomial.cpp` and a Makefile rule.
- [ ] **2. `Polynomial<T>` core type.**
  - Dense coefficient vector, lowest degree first; invariant: no trailing zero coefficients (the zero polynomial is empty).
  - `degree()` (−1 or a sentinel for zero), `+ − ×`, scalar multiplication, equality, canonical `toString()` that core can parse (`(3 * (x ^ 2)) + ...`).
  - *Tests:* ring identities on random samples; degree rules.
- [ ] **3. Division and gcd.** Division with remainder over a field (`T = double` or field elements). Euclidean gcd, made monic. Throws `std::invalid_argument` on division by the zero polynomial.
- [ ] **4. Evaluation and calculus.** Horner evaluation, derivative, antiderivative, composition `p(q(x))`.
- [ ] **5. Expressions ↔ polynomials.**
  - `from_expression(expr, var)` recognises a polynomial tree in one variable (coefficients may be any expression free of `var`), and throws if the tree isn't polynomial.
  - `to_expression()` builds the tree with `core::build`.
  - With these, core's `simplify` can collect like terms (`x + x → 2*x`, `x*x → x^2`) through the normal form ([core](../core/plan.md) roadmap item 7).
- [ ] **6. Roots.**
  - Closed forms for degree ≤ 2.
  - Newton/Halley from [numerics](../numerics/plan.md) for real roots.
  - All roots later via the companion matrix's eigenvalues ([linalg](../linalg/plan.md)).
  - This feeds the root plan's "solving" track: `solve(p = 0, x)`.
- [ ] **7. Interpolation:** Lagrange and Newton forms through given points.

## Performance

- Batch Horner evaluation of one polynomial at many points with SIMD (one lane per point). Scalar Horner is the baseline; benchmark in `benchmarks/algebra/`.
- Multiplication: schoolbook first; Karatsuba once a benchmark shows large degrees matter.

## Connections

- **Offers:** element type for [abstract_algebra](../abstract_algebra/plan.md)'s `PolynomialRing`; normal form for [core](../core/plan.md) `simplify`; polynomial calculus for [calculus](../calculus/plan.md) (Taylor polynomials).
- **Needs:** [core](../core/plan.md) (expression conversion), [numerics](../numerics/plan.md) and [linalg](../linalg/plan.md) (roots).

## Decisions needed

- **Coefficient type:** templated `T` with `double` as the main instantiation, or only `double` at first? Templating keeps finite-field coefficients possible.
- **Sparse polynomials** (`x^1000 + 1`): needed, or dense only?
- **Multivariate polynomials:** out of scope for now, or planned (needed for Gröbner bases, Galois theory)?
