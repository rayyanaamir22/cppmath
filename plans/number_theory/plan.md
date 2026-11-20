# number_theory plan

_Last updated 2026-11-20 · status: **early**_

Integers and their arithmetic: primes, divisibility, modular arithmetic and factorisation. It supplies [abstract_algebra](../abstract_algebra/plan.md) (finite fields need primality and inverses) and [combinatorics](../combinatorics/plan.md) (gcd for exact binomials), and is the natural home of an exact `Integer` for [core](../core/plan.md).

## Status

- `prime.hpp`: `is_prime`, `generate_primes` (sieve of Eratosthenes), `prime_factorization`; `test_prime`.

**Known bugs**
- **`gcd`, `lcm` and `next_prime` are unimplemented.** They throw `std::runtime_error("... not yet implemented")`, and `test_prime` asserts that they throw, so the tests pass while the functions don't work.
- **`gcd_lcm(numbers)`** is marked "AI generated prototype" and calls the unimplemented `gcd` / `lcm`, so it always throws.
- **Error types:** errors use `std::runtime_error`, against the CLAUDE.md convention.
- **Duplication:** abstract_algebra re-implements number theory: `FiniteField` has its own trial-division `checked_prime` and an extended-Euclid inverse in `field/Field.cpp`.

## Roadmap

- [ ] **1. Make it work.**
  - `gcd`: Euclid, plus binary GCD as a benchmarked alternative.
  - `lcm`: overflow-checked, as `a / gcd(a, b) * b`.
  - `next_prime`: the smallest prime > n, throwing past the largest 64-bit prime.
  - `gcd_lcm`: rewrite.
  - Errors switch to `std::invalid_argument` / `std::out_of_range` (e.g. `gcd(0, 0)` per the decision below).
  - *Tests:* replace the "throws" assertions with values (`gcd(48, 18) == 6`, `lcm(4, 6) == 12`, `next_prime(10) == 11`, `next_prime(17) == 19`).
- [ ] **2. Modular arithmetic.**
  - Extended Euclid (Bézout coefficients); modular inverse; modular exponentiation (`unsigned __int128` for the products).
  - Then make `FiniteField` use `is_prime` and `mod_inverse` from here instead of its private copies.
- [ ] **3. More arithmetic functions:** CRT for pairwise-coprime moduli (and the general case), Euler φ, Möbius μ, divisor count and sum, Jacobi/Legendre symbols, primitive roots.
- [ ] **4. Fast primality and factoring:** deterministic Miller–Rabin for all 64-bit inputs (fixed witness set), Pollard's rho with Brent's cycle detection, a segmented sieve for large ranges.
- [ ] **5. Exact integers (design):** an `Integer` type (arbitrary precision, or checked 64-bit first) shared with [combinatorics](../combinatorics/plan.md) and offered to [core](../core/plan.md) as an exact numeric sort (core roadmap item 5), with `Rational` on top.

## Performance

- `constexpr` small-prime tables and a `constexpr` `is_prime` for compile-time constants.
- Benchmarks: binary vs Euclidean GCD; trial division vs Miller–Rabin crossover; sieve vs segmented sieve (`benchmarks/number_theory/`).
- Batched modular multiplication with SIMD for F_p arithmetic, shared with [abstract_algebra](../abstract_algebra/plan.md).

## Connections

- **Offers:** primality, gcd and inverses to [abstract_algebra](../abstract_algebra/plan.md) and [combinatorics](../combinatorics/plan.md); `Integer`/`Rational` to [core](../core/plan.md); irreducibility tests over F_p for GF(pⁿ) (with [algebra](../algebra/plan.md) polynomials).
- **Needs:** nothing (a leaf module).

## Decisions needed

- **`gcd(0, 0)`:** return 0 (the common convention) or throw?
- **Integer width:** stay `unsigned long long` with checked overflow, add signed versions, or build arbitrary precision now?
- **Dependency direction:** confirm `abstract_algebra → number_theory` (fields import primality and inverses from here).
