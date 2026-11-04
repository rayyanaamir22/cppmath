# cppmath Plan

Living roadmap: current status, known issues, and what's next. Conventions and build instructions are in [CLAUDE.md](CLAUDE.md).

_Last reviewed: 2026-11-04 (branch `core`)._

## Module status

| Module | Status | Tests | Notes |
|---|---|---|---|
| `abstract_algebra` | Group, Ring, Field, VectorSpace, Lie: abstract CRTP bases + example concrete types (S3, Z/nZ, FiniteField mod p) | `test_finite_field` ✅ | Demos for each structure. `*Element.cpp` files are empty stubs. |
| `sets` | Interval, Finite, Union, Intersection, Empty, Universal | via `test_expression` (smoke only) | `sets.hpp`/`sets.cpp` are empty stubs. Has no dedicated doctest suite. |
| `core` (symbolic) | `Expression`, `Variable`, `Constant`, Add/Multiply/Power, Sin/Cos/Exp/Log, `Function` | `test_expression` (print-only) | Variables can't be evaluated yet. There's no environment/binding. |
| `linalg` | `Vector` (AVX add/sub/+=/-=), `Matrix<T>` (header-only, `operator*`) | `test_matrix` ✅, `test_vector` ❌ (doesn't build) | `decomposition.cpp` is empty. |
| `numerics` | Newton sqrt / cbrt | `test_root` ✅ | Has a benchmark. |
| `number_theory` | `is_prime` and friends | `test_prime` ✅ | |
| `combinatorics` | combinations / permutations | `test_combo` ✅ | |
| `graph` | adjacency graph, `is_tree` | `test_graph` ✅ | |
| `calculus`, `geometry`, `stats`, `algebra` (polynomial), `utils` | Empty stubs | none | |

## Known issues (fix first)

1. **`linalg/vector.hpp` doesn't compile.** It includes `"../abstract_algebra/VectorElement.hpp"`, but the file is at `abstract_algebra/vector/VectorElement.hpp`. This breaks `test_vector`, `vector_demo`, the SIMD benchmark, and therefore `make test` and `make all`. Fix: `#include "cppmath/abstract_algebra/vector/VectorElement.hpp"`.
2. **`tests/core/test_core.cpp` and `test_expression.cpp` are byte-identical.** Neither uses doctest (both just print). Delete one and convert the other to real `CHECK`s.
3. **The SIMD paths never run on the dev machine (arm64).** They're AVX-only, so on Apple Silicon `vectorize=true` silently falls back to scalar and the benchmark compares scalar to scalar.
4. **Makefile gaps.** `lie_demo` isn't in `EXAMPLES` and hardcodes its own flags. `matrix_demo`, `graph_demo`, and `ml_pipeline` have no rules. The benchmark pattern rule always links `src/linalg/vector.cpp`, whatever the benchmark needs. Consider wildcard/pattern rules or a move to CMake.
5. **`Field.hpp` has an open question in a comment** ("is this not a circular dependency?") about `FieldElement.hpp`. Check it and either resolve it or document the answer.

## Next up

### 1. Symbolic core → calculus (current branch focus)
- [ ] Convert `test_expression` to doctest assertions.
- [ ] Evaluation environment: `evaluate(const Env&)` that binds `Variable` names to values (replaces "no variables" `evaluate()`).
- [ ] Missing nodes: Subtract, Divide, Negate (or express via Add/Multiply/Power with constants).
- [ ] `Operator` base with arbitrary arity (planned in the old roadmap, not started).
- [ ] Simplification pass (constant folding, `x*1`, `x+0`, `x^1`).
- [ ] Symbolic differentiation `d/dx` on the expression tree → fills `calculus/diff.hpp`.
- [ ] Use `Function`'s domain set: check that inputs lie in the domain on evaluate, and validate the domain of `Log`.

### 2. AST parser
- [ ] Tokenizer + recursive-descent (Pratt) parser from string → `Expression` tree. It should round-trip `toString()`.
- [ ] Parse set notation (`[0, 1)`, `{1, 2}`, `A U B`, `A ∩ B`) into `Set` trees.

### 3. Sets
- [ ] Dedicated `tests/sets/test_sets.cpp` doctest suite (union/intersection/complement, interval merging, `equals`).
- [ ] Use sets as `Function` domain and codomain everywhere.

### 4. Linear algebra + SIMD
- [ ] Fix the include (Known issue 1), then add ARM NEON paths to `Vector` (`float64x2_t`, `vaddq_f64`/`vsubq_f64`).
- [ ] Factor the per-ISA code behind a small internal helper (`detail/simd.hpp`) so each op isn't three copies of the loop.
- [ ] SIMD `dot_product` (horizontal reduction) and `magnitude`. Use FMA (`_mm256_fmadd_pd` / `vfmaq_f64`) where it's available.
- [ ] Matrix: SIMD + cache-blocked `operator*`, transpose, then decompositions (LU, QR) in `decomposition.cpp`.
- [ ] Benchmarks: run each case several times, report min/median, prevent dead-code elimination. Record results in `benchmarks/benchmarks.md`.

### 5. Abstract algebra
- [ ] Polynomial rings (fills `algebra/polynomial.cpp`).
- [ ] Quotient structures (quotient groups, rings, fields).
- [ ] Modules over rings.
- [ ] Galois theory: field extensions, Galois groups.
- [ ] Representation theory: group representations, which ties into `linalg::Matrix`.
- [ ] Category theory: categorical abstractions over the existing structures (exploratory).

## Optimization ideas
- **Cayley tables for finite structures.** Precompute the operation tables for finite groups and fields as a flat `n×n` array indexed by element ordinals. This turns repeated composition into O(1) lookups and trades memory for speed. It would also make a good benchmark target.
- **SIMD/vectorization** beyond `Vector`: matrix ops, batched modular arithmetic for finite fields, and vectorised expression evaluation over many inputs.
- **Lazy evaluation / expression templates** for chains of vector and matrix operations, to avoid temporaries.
- **Parallelism** (`std::execution`, threads) for large matrix and graph computations.
- **constexpr** for small number-theory and combinatorics routines (primes, binomials).

## Long-term vision
The AST-parseable string forms of sets and expressions are groundwork for a single symbolic calculus engine: parse → manipulate → differentiate and integrate → evaluate over a domain set. Abstract algebra and linear algebra connect to it through polynomial rings and representations.
