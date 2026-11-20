# utils plan

_Last updated 2026-11-20 · status: **support module**_

Small helpers shared across modules. **Rule:** something belongs in utils only when at least two modules need it. Anything used by one module stays in that module.

## Status

- `utils/format.hpp`: `format_number(double)`, the shortest decimal text that parses back to exactly the same double. Used by [core](../core/plan.md)'s printer and [sets](../sets/plan.md)' `toString`; tested through `test_sets` and `test_expression`.
- `include/cppmath/utils.hpp`: an empty stub (comments only), separate from the `utils/` folder.

## Roadmap

- [ ] **1. `utils/simd.hpp`, the shared per-ISA helper.**
  - Thin wrappers for loading, storing, adding, multiplying and FMA on doubles, for AVX (x86_64), NEON (arm64) and scalar.
  - Plus a loop helper that runs the vector body over full lanes and a scalar tail.
  - First user: [linalg](../linalg/plan.md) item 1; then [stats](../stats/plan.md) and [numerics](../numerics/plan.md).
  - *Tests:* each wrapper against scalar arithmetic on every compiled ISA.
- [ ] **2. Floating-point comparison.** `approx_equal(a, b, abs_tol, rel_tol)`, with infinities equal to themselves, replacing the separate `double_eq` / `EPSILON` copies in `src/sets/*.cpp` (and similar ones elsewhere). Optionally `ulp_distance`.
- [ ] **3. Benchmark harness.** A tiny header for `benchmarks/`: run a callable N times, report min, median and mean, and stop the compiler from removing the work (a `do_not_optimize` barrier). Used by every benchmark, so results are comparable and can be recorded in [benchmarks/benchmarks.md](../../benchmarks/benchmarks.md).
- [ ] **4. A span-like view** (`utils/view.hpp`), if [stats](../stats/plan.md) and [numerics](../numerics/plan.md) both want to accept "pointer + length" without copying (C++17 has no `std::span`).
- [ ] **5. Remove or repurpose `utils.hpp`.** It has no users. Delete it, or make it an umbrella header that includes `utils/*.hpp`.

## Performance

- The SIMD helper must add no overhead: check that the wrapped loop compiles to the same instructions as the hand-written intrinsics in `linalg/vector.cpp` (compare `-O3` assembly once), and benchmark both.

## Connections

- **Offers:** formatting (core, sets), SIMD (linalg, stats, numerics, geometry), comparison (sets, geometry, numerics), benchmarking (all `benchmarks/`).
- **Needs:** nothing (a leaf module).

## Decisions needed

- **Where the SIMD helper lives:** `utils/simd.hpp` (shared, recommended) or `linalg/detail/simd.hpp` (linalg-only, as the old root plan suggested)?
- **`utils.hpp`:** delete, or turn into an umbrella header?
