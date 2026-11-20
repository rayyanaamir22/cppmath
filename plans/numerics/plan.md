# numerics plan

_Last updated 2026-11-20 · status: **early** · scope beyond the root functions is *proposed*_

Numerical methods: root finding, quadrature, ODE solvers and floating-point care. It is the numerical fallback behind the symbolic engine: when [calculus](../calculus/plan.md) can't integrate symbolically, or the solving track can't solve in closed form, numerics does it to a tolerance. Methods should accept core expressions as well as plain C++ callables.

## Status

- `root.hpp`: `newton_sqrt(x, tolerance, max_iterations)` and `newton_cbrt(...)`. Negative input to `newton_sqrt` throws `std::invalid_argument`; a TODO notes complex roots for later.
- `test_root`, `bench_root_newton` (Newton vs `std::sqrt`: speed and mean absolute error).

## Roadmap *(proposed)*

- [ ] **1. Root finding** on `f: double → double` and on a core `Expression` in one variable:
  - bisection (bracketing, guaranteed);
  - Newton, where for an `Expression` the derivative comes from `calculus::differentiate`, so no hand-written derivative is needed;
  - secant; Brent's method (bracketing and fast, the default).
  - All of them report non-convergence by throwing, with the iteration count.
  - *Tests:* polynomials with known roots, multiple roots (Newton's slowdown), functions without a bracket.
- [ ] **2. Quadrature:** trapezoid and Simpson (composite), Gauss–Legendre (fixed orders), adaptive Simpson / Gauss–Kronrod with an error estimate. This is the numerical fallback for [calculus](../calculus/plan.md)' integral.
- [ ] **3. ODE solvers:** explicit Euler, RK4, and adaptive RK45 (Dormand–Prince) for systems `y' = f(t, y)` over `linalg::Vector`.
- [ ] **4. Numerical differentiation:** central differences with Richardson extrapolation. Used to cross-check calculus, as `test_diff` already does by hand.
- [ ] **5. Floating-point helpers** (shared through [utils](../utils/plan.md)): ULP distance and `approx_equal`, Kahan/Neumaier compensated summation, and a guide to tolerances.
- [ ] **6. Interpolation and approximation:** cubic splines, Chebyshev approximation (complementing [algebra](../algebra/plan.md)'s polynomial interpolation).
- [ ] **7. Complex roots:** once complex numbers exist (see [core](../core/plan.md) item 5), lift the `newton_sqrt` negative-input TODO.

## Performance

- **Batch evaluation of a core expression** at many points: compile the tree to a flat instruction sequence and evaluate lanes with SIMD. Quadrature and plotting benefit most; shared with [core](../core/plan.md).
- `bench_root_newton` already exists; extend it with a fast inverse-sqrt style approximation as a learning comparison, and record results in [benchmarks/benchmarks.md](../../benchmarks/benchmarks.md).

## Connections

- **Needs:** [core](../core/plan.md) (expressions), [calculus](../calculus/plan.md) (derivatives), [linalg](../linalg/plan.md) (vectors for ODE systems), [utils](../utils/plan.md) (floating-point helpers).
- **Offers:** numerical integration and ODEs to [calculus](../calculus/plan.md); roots to [algebra](../algebra/plan.md) and the solving track in the root [PLAN.md](../../PLAN.md); special functions (erf, Γ) to [stats](../stats/plan.md).

## Decisions needed

- **Interface:** template on a callable (`F f`) for speed, `std::function` for simplicity, or both? Expression-taking overloads wrap a compiled evaluator.
- **Errors:** throw on non-convergence (consistent with the rest of the library), or return a result struct with a status and the best estimate?
- **Special functions** (erf, Γ, Bessel): here or in stats?
