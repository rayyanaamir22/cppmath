# combinatorics plan

_Last updated 2026-11-20 · status: **early** · scope beyond the fixes is *proposed*_

Counting and enumeration: factorials, binomial coefficients, sequences, permutations and combinations. It supplies [abstract_algebra](../abstract_algebra/plan.md) (Sₙ), [stats](../stats/plan.md) (the binomial distribution) and [graph](../graph/plan.md) (counting).

## Status

- `combo.hpp`: `fibonacci(n)`, `factorial(n)`, `fibonacci_sequence(limit)`, `binomial_coefficient(n, k)`; `test_combo`.

**Known bugs**
- **`binomial_coefficient` overflows.** It computes `n! / (k! (n−k)!)`, so `factorial` overflows `unsigned long long` for n > 20 and the result is silently wrong, even though C(n, k) itself fits: `binomial_coefficient(25, 2)` returns **0** instead of 300 (checked 2026-11-20).
- **No overflow checks anywhere:** `factorial(21)` and large `fibonacci(n)` wrap around silently.
- **Dead checks:** `factorial` and `binomial_coefficient` test `n < 0` on unsigned parameters, which is always false, and `binomial_coefficient`'s message ("n, k cannot be zero") describes neither the test nor a real error.
- **Indexing:** `fibonacci` is 1-based (`fibonacci(1) == 0`, `fibonacci(2) == 1`) and `fibonacci(0)` throws. The standard convention is F₀ = 0, F₁ = 1 (see Decisions).
- **Error types:** errors use `std::runtime_error`, against the CLAUDE.md convention (`std::invalid_argument` / `std::out_of_range`).
- **Threads:** `fibonacci` memoises in a function-local `static std::vector`, which is not thread-safe.
- **Recursion:** `factorial` is recursive, which is unnecessary.

## Roadmap

- [ ] **1. Correctness pass.**
  - Compute binomials with the multiplicative formula, using a gcd reduction at each step, and throw `std::overflow_error` (or `out_of_range`, per the decision) when the result doesn't fit.
  - Checked `factorial` (throws past 20!), iterative.
  - Remove the dead checks; switch to the conventional exception types.
  - Make the `fibonacci` memo thread-safe or drop it.
  - *Tests:* boundary values (`C(n, 0)`, `C(n, n)`, `C(67, 33)`, which fits, `C(68, 34)`, which overflows); overflow throws.
- [ ] **2. More counting functions** *(proposed)*: nPk, multinomial coefficients, Catalan numbers, Stirling numbers of both kinds, Bell numbers, integer partitions p(n), derangements.
- [ ] **3. Generators** *(proposed)*: iterate permutations, k-combinations and subsets in lexicographic order. These feed [abstract_algebra](../abstract_algebra/plan.md)'s Sₙ.
- [ ] **4. Big results** *(proposed)*: an arbitrary-precision path (or `long double`/log-space approximations) for counts that don't fit 64 bits. Decide together with [number_theory](../number_theory/plan.md)'s Integer type.
- [ ] **5. Expressions** *(proposed)*: `factorial` and `binomial` as operators in core's signature, with interpretations. A postfix `n!` needs `Notation::Postfix` ([core](../core/plan.md) item 9).

## Performance

- `constexpr` tables for factorials up to 20! and binomials up to row 67 (everything that fits 64 bits), so lookups are compile-time constants. Benchmark the table against the multiplicative formula (`benchmarks/combinatorics/`).
- Pascal-row computation for many binomials at once.

## Connections

- **Offers:** permutations to [abstract_algebra](../abstract_algebra/plan.md); binomials to [stats](../stats/plan.md) and [calculus](../calculus/plan.md) (Taylor coefficients); spanning-tree and colouring counts to [graph](../graph/plan.md).
- **Needs:** [number_theory](../number_theory/plan.md) (gcd, for exact binomial reduction).

## Decisions needed

- **Fibonacci indexing:** switch to the standard F₀ = 0, F₁ = 1 (a breaking change to `test_combo`), or keep 1-based and document it?
- **Overflow policy:** throw (`std::overflow_error` or `out_of_range`), saturate, or return an optional?
- **Arbitrary precision:** build a big-integer type here or in number_theory, or depend on none and stay within 64 bits?
