# calculus plan

_Last updated 2026-11-20 · status: **active**_

Calculus on symbolic expressions: differentiation now; integration, limits, series and multivariable calculus next. It is the main consumer of [core](../core/plan.md)'s AST, and the "manipulate" step of the root plan's parse → manipulate → solve pipeline.

How differentiation works (rule table keyed by operator name, chain rule, `simplify` afterwards) is documented in `include/cppmath/calculus/diff.hpp` and in the "Consumers outside core" section of [docs/architecture/core.md](../../docs/architecture/core.md). Create `docs/architecture/calculus.md` when integration lands.

## Status

- **`differentiate(expr, var, rules)`:**
  - `DerivativeRules::standard()` covers `+ − × / ^` (three power-rule cases), `neg`, `sin cos tan exp log sqrt abs` and the constants `pi e inf`.
  - Custom operators get rules by copying the table and calling `define`.
  - Differentiates under `sum` when its bounds don't depend on `var`.
  - The result is simplified.
- **Errors:** relations, logic, sets and `prod` have no rule and throw `std::invalid_argument`.
- **Tests:** `test_diff` (11 cases), including agreement with numerical derivatives on 23 functions and second derivatives. `core_demo` shows it.

## Roadmap

- [ ] **1. Several variables.**
  - `gradient(expr, vars)`, `jacobian(exprs, vars)`, `hessian(expr, vars)` returning expressions (or a matrix of expressions once [linalg](../linalg/plan.md) has an interpretation).
  - `core::Function` with several parameters, together with [core](../core/plan.md).
  - *Tests:* compare against finite differences, as `test_diff` does.
- [ ] **2. `prod`:** the logarithmic derivative `prod · sum(f'/f)`, with a documented failure when a factor is zero.
- [ ] **3. Taylor polynomials:** `taylor(expr, var, a, n)` builds `sum(k, 0, n, f^(k)(a) / k! * (x - a)^k)` as an expression, or as a [Polynomial](../algebra/plan.md) once algebra exists.
- [ ] **4. Limits:** numerical first (one-sided limits by sequences, with tolerance), then symbolic rules for indeterminate forms (L'Hôpital through `differentiate`).
- [ ] **5. Integration (design).**
  - An integral binder `integral(x, a, b, f)` (definite) and `antiderivative(f, x)` (indefinite), via [core](../core/plan.md) roadmap item 3.
  - A symbolic rule table: linearity, polynomials, the elementary functions, simple substitutions.
  - A numerical fallback through [numerics](../numerics/plan.md) quadrature when no symbolic rule applies.
  - *Done when:* d/dx of every symbolic antiderivative matches the integrand (differentiate as the checker), and numerical integrals match closed forms.
- [ ] **6. ODEs:** an expression-level front end (`y' = f(x, y)` given as an expression) over [numerics](../numerics/plan.md)' solvers.
- [ ] **7. Piecewise and conditional functions:** derivative of `if(cond, a, b)` once core has it ([core](../core/plan.md) item 4); `abs` via its piecewise definition.

## Performance

- Differentiation output can grow fast (product rule chains). Measure the size of `node_count(differentiate(...))` before and after `simplify` on a corpus; consider memoising shared subexpressions only if trees grow large.
- Numerical integration evaluates the integrand many times. Batch evaluation (shared with [numerics](../numerics/plan.md) and core) is where SIMD pays.

## Connections

- **Needs:** [core](../core/plan.md) (AST, binders, simplify), [numerics](../numerics/plan.md) (quadrature, ODE solvers), [algebra](../algebra/plan.md) (polynomials), [linalg](../linalg/plan.md) (Jacobians/Hessians as matrices).
- **Offers:** derivatives to [numerics](../numerics/plan.md) (Newton's method on expressions) and to the solving track in the root [PLAN.md](../../PLAN.md).

## Decisions needed

- **Integral syntax:** `integral(x, a, b, f)` (binder style, matching `sum`) or `integral(f, x, a, b)`?
- **Scope of symbolic integration:** a rule table plus simple substitution, or something closer to a Risch-style algorithm for elementary functions? (A table is recommended for this project.)
- **Where `gradient`/`jacobian` return values live:** a vector of expressions, or a matrix expression that needs core's overload resolution?
