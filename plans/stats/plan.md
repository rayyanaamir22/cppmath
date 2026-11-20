# stats plan

_Last updated 2026-11-20 · status: **stub** · scope below is *proposed*_

Probability and statistics: descriptive statistics, probability distributions, sampling and regression. Reductions over large arrays (sums, variances, correlations) make it one of the best SIMD targets in the library, after linalg.

## Status

- `include/cppmath/stats/stats.hpp` and `src/stats/stats.cpp` are empty (comments only). No tests or Makefile rule.

## Roadmap *(proposed)*

- [ ] **1. Descriptive statistics** on a span of doubles:
  - mean;
  - variance and standard deviation, with sample (n−1) and population variants, computed by Welford's online algorithm for numerical stability;
  - min, max, median and quantiles (with a stated interpolation method), mode;
  - covariance and Pearson correlation.
  - *Tests:* known datasets; stability on large-offset data (where the naive formula fails).
  - Add `tests/stats/test_stats.cpp` and Makefile rules.
- [ ] **2. SIMD reductions:** vectorised sum, mean, variance and dot-product-based covariance through [utils](../utils/plan.md)' SIMD helper (AVX and NEON). Compare against the scalar version, and benchmark in `benchmarks/stats/`.
- [ ] **3. Distributions:** uniform, normal, binomial (via [combinatorics](../combinatorics/plan.md)), Poisson, exponential.
  - `pdf`/`pmf`, `cdf`, `quantile`, `mean`, `variance`.
  - Sampling through `<random>` engines.
  - A common interface (decision below). The normal cdf needs `erf` (numerics or `std::erf`).
- [ ] **4. Regression:** ordinary least squares via [linalg](../linalg/plan.md)'s QR; simple linear regression with r².
- [ ] **5. Later:** hypothesis tests (z, t, χ²), confidence intervals, kernel density estimates, Monte Carlo integration (a cross-check for [calculus](../calculus/plan.md) integrals).

## Performance

- SIMD reductions (item 2) with careful handling of summation order. The tests state their tolerance, since vectorised sums round differently.
- One-pass (streaming) algorithms throughout, so large datasets are read once.

## Connections

- **Needs:** [linalg](../linalg/plan.md) (QR for regression), [combinatorics](../combinatorics/plan.md) (binomials), [numerics](../numerics/plan.md) (special functions, quadrature for continuous expectations), [utils](../utils/plan.md) (SIMD helper).
- **Offers:** examples and benchmark workloads for SIMD; probability for [sets](../sets/plan.md) (supports) and [calculus](../calculus/plan.md) (expectations as integrals).

## Decisions needed

- **Input type:** `std::vector<double>`, pointer + length, or a span-like view? (C++17 has no `std::span`; a small view type in utils would do.)
- **Distribution interface:** a virtual `Distribution` base (polymorphic, like `sets::Set`) or plain value types with free functions?
- **Symbolic statistics** (expectations as core expressions) in scope, or numeric only?
