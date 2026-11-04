# Benchmarks

Timing harnesses for cppmath modules, mostly SIMD vs. scalar comparisons. Layout: `benchmarks/<module>/bench_*.cpp`.

```sh
make benchmarks       # builds every benchmarks/*/*.cpp with -O3 (+ -mavx on x86_64)
make run-benchmarks
```

| Benchmark | What it measures |
|---|---|
| `linalg/bench_vector_simd` | `Vector` add/sub with `vectorize=false` vs `true`, N = 1e6 |
| `numerics/bench_root_newton` | `newton_sqrt` vs `std::sqrt` (speed + mean abs error) |

**Caveat:** the vector SIMD paths are AVX-only for now. On arm64 (Apple Silicon) both runs are scalar. See PLAN.md (NEON support).

## Results

_Record runs here (machine, compiler, flags, numbers) so before/after comparisons are possible._
