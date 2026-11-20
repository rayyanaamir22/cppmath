# linalg plan

_Last updated 2026-11-20 · status: **active**_

Numerical linear algebra: vectors, matrices, decompositions and solvers. It is the main **SIMD/performance** module of cppmath, and the reference for how every SIMD path in the library is written and benchmarked (see the SIMD section of [CLAUDE.md](../../CLAUDE.md)).

No architecture doc yet. Create `docs/architecture/linalg.md` when the SIMD helper and decompositions land.

## Status

- **`Vector`:** concrete class of `double`.
  - `vector_add` / `vector_sub` have an AVX path behind a `vectorize` flag, with scalar fallback and tail loop.
  - Operators, dot and cross products, magnitude and normalise are scalar.
  - Bounds-checked `operator[]`.
- **`Matrix<T>`:** header-only, row-major flat storage, unchecked `operator()`, naive triple-loop `operator*` that throws on a dimension mismatch.
- **Empty files:** `src/linalg/decomposition.cpp`; `src/linalg/matrix.cpp` (intentionally, since Matrix is header-only).
- **Tests and demos:** `test_vector`, `test_matrix`, `bench_vector_simd`; examples `vector_demo`, `matrix_demo`, `ml_pipeline`.

**Known issues** *(moved from the root plan)*
- **SIMD never runs on the dev machine.** The paths are AVX-only, so on Apple Silicon (arm64) `vectorize = true` falls back to scalar, and `bench_vector_simd` compares scalar with itself (identical timings).
- **Compound operators** `+=` / `-=` have no SIMD / `vectorize` option, unlike `vector_add` / `vector_sub`.
- **`Vector` is disconnected from `abstract_algebra::VectorSpace`** (it used to inherit `VectorElement`). Shared with [abstract_algebra](../abstract_algebra/plan.md), item 9.

## Roadmap

- [ ] **1. NEON and a shared SIMD helper.**
  - Add ARM NEON paths (`float64x2_t`, `vaddq_f64` / `vsubq_f64`) next to AVX.
  - Move the per-ISA loads, stores and loops into [utils](../utils/plan.md)' `utils/simd.hpp`, so each operation isn't three copies of the loop.
  - Give `+=` / `-=` the same paths.
  - *Tests:* every SIMD path matches the scalar path on lengths 0…17 (tails).
  - *Done when:* the benchmark shows a real speed-up on arm64.
- [ ] **2. Reductions.** SIMD `dot_product` (horizontal reduction) and `magnitude`, with FMA (`_mm256_fmadd_pd` / `vfmaq_f64`) where available. The tests compare against the scalar path within a stated tolerance, since summation order changes rounding.
- [ ] **3. Matrix basics.** Optional bounds checking (an `at()` that throws), `transpose`, `identity`, `+ −`, scalar multiplication, `Matrix × Vector`, equality, `toString`.
- [ ] **4. Fast matrix multiply.** Cache-blocked (tiled) multiply, then SIMD inner kernels; keep the naive version as the baseline. Benchmark sizes 64…1024 (`benchmarks/linalg/bench_matmul.cpp`).
- [ ] **5. Decompositions and solvers** (fills `decomposition.cpp`):
  - LU with partial pivoting → `solve`, `determinant`, `inverse`;
  - QR by Householder reflections → least squares (for [stats](../stats/plan.md) regression);
  - Cholesky for symmetric positive-definite matrices.
  - *Tests:* reconstruct A from the factors; known solutions; singular matrices throw.
- [ ] **6. Eigenvalues.** Power iteration (dominant eigenpair), then the QR algorithm with shifts for small dense matrices. Needed by [graph](../graph/plan.md) (spectra) and [algebra](../algebra/plan.md) (companion-matrix roots).
- [ ] **7. Core interpretation (design; needs core's overload resolution).** Matrix and vector sorts, literals and an `Interpretation` over `Matrix<double>`, so `A * x = b` can be parsed and evaluated. Owned jointly with the [core plan](../core/plan.md), items 1–2.
- [ ] **8. Expression templates and parallelism.** Lazy evaluation of `a + b + c` without temporaries, and multithreaded large multiplies, each only if a benchmark justifies it.
- [ ] **9. Benchmark discipline** (applies to every item above):
  - run each case several times and report min and median;
  - prevent dead-code elimination;
  - record machine, compiler, flags and numbers in [benchmarks/benchmarks.md](../../benchmarks/benchmarks.md).

## Performance

This whole module is the performance track. The rules: every SIMD path has a scalar twin, a test that compares them, and a benchmark. AVX and NEON are both implemented, because development is on arm64. Alignment is not assumed (unaligned loads), unless storage is known to be aligned.

## Connections

- **Offers:** matrices and solvers to [stats](../stats/plan.md) (regression), [geometry](../geometry/plan.md) (transforms), [graph](../graph/plan.md) (spectral), [abstract_algebra](../abstract_algebra/plan.md) (representations), [algebra](../algebra/plan.md) (roots), [calculus](../calculus/plan.md) (Jacobians), and [core](../core/plan.md) (a matrix interpretation).
- **Needs:** [utils](../utils/plan.md) (SIMD helper, benchmark harness).

## Decisions needed

- **`Vector` and `Matrix<T>`:** should `Vector` become `Matrix`-compatible (a column matrix), or stay separate with `Matrix × Vector` only?
- **Templating `Vector`:** template it on the scalar type like `Matrix<T>`, or keep it `double`-only for SIMD simplicity?
- **Error policy for `operator()`:** keep it unchecked (fast) with `at()` checked, as `std::vector` does?
- **Relation to `VectorSpace`:** decide together with [abstract_algebra](../abstract_algebra/plan.md).
