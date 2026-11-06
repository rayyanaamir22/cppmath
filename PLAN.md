# cppmath Plan

Living roadmap: current status, known issues, and what's next. Conventions and build instructions are in [CLAUDE.md](CLAUDE.md).

_Last reviewed: 2026-11-06 (branch `core`)._

## Module status

| Module | Status | Tests | Notes |
|---|---|---|---|
| `abstract_algebra` | Group, Ring, Field, VectorSpace, Lie: abstract CRTP bases + example concrete types (S3, Z/nZ, FiniteField mod p) | `test_finite_field` ✅ | Demos for each structure. `*Element.cpp` files are empty stubs. |
| `sets` | Interval, Finite, Union, Intersection, Empty, Universal | `test_sets` ✅ | `sets.hpp`/`sets.cpp` are empty stubs. `FiniteSet` is immutable (`with_element` / `without_element`). |
| `core` (symbolic) | Generic `Apply(Operator, args)` AST over a `Signature`; `fold`/`transform`, `Substitution`, `Interpretation`/`Environment`/`evaluate`, Lexer + Pratt `Parser`, canonical printer, `Function` | `test_signature`, `test_expression`, `test_traversal`, `test_evaluate`, `test_parser` ✅ | Milestones 1-4 of [core/PLAN.md](include/cppmath/core/PLAN.md) done. Sets/binders/second carrier not started. |
| `linalg` | `Vector` (AVX add/sub), `Matrix<T>` (header-only, `operator*`) | `test_matrix` ✅, `test_vector` ✅ | `decomposition.cpp` is empty. |
| `numerics` | Newton sqrt / cbrt | `test_root` ✅ | Has a benchmark. |
| `number_theory` | `is_prime` and friends | `test_prime` ✅ | |
| `combinatorics` | combinations / permutations | `test_combo` ✅ | |
| `graph` | adjacency graph, `is_tree` | `test_graph` ✅ | |
| `calculus`, `geometry`, `stats`, `algebra` (polynomial), `utils` | Empty stubs | none | |

## Known issues

Resolved 2026-11-04 (all 8 test targets and all examples/benchmarks now build; `make test` is green):
- ~~`linalg/vector.hpp` didn't compile~~: it pointed at a stale `VectorElement` path, and `VectorElement` had since become a CRTP template that `Vector` no longer matched. `Vector` is now a standalone concrete class. `add()` / `scalar_multiply()` are kept as named equivalents of `+` and `*` and return by value (they used to return leaked raw pointers). `operator+=` / `-=` had definitions with a stray `bool` parameter that didn't match the header; they're now plain scalar loops.
- ~~Duplicate print-only core tests~~: `test_core.cpp` deleted; `test_expression.cpp` is now a doctest suite (19 assertions).
- ~~Makefile gaps~~: `lie_demo`, `matrix_demo`, `graph_demo`, `ml_pipeline` have rules and are in `EXAMPLES`; each benchmark has its own dependency rule; `make clean` removes benchmark binaries too.

Resolved 2026-11-06 (found while documenting the headers; all covered by new tests):
- ~~`IntervalSet::set_union` merged `(0,1) U (1,2)` into `(0,2)`~~, wrongly including 1. Touching ends now merge only if one of them contains the shared point. The same pass fixed shared-endpoint inclusivity (union takes either, intersection requires both) and `equals()` for infinite bounds.
- ~~`FiniteSet` tolerance was inconsistent~~: `contains`/`equals` used 1e-9 but `intersect`/`set_union` compared exactly. All now use 1e-9. The mutating `add_element` / `remove_element` became `with_element` / `without_element`.
- ~~`toString()` of sets printed 6 digits~~, so it couldn't round-trip. Sets now use `utils::format_number` (shortest text that parses back exactly), shared with `core`.
- ~~`Ring`/`Field::identity()` returned 1 while `operate()` is addition~~. Identities now live on `AlgebraicOperation` (`has_identity()` / `identity(structure)`); `identity()` is the additive one (0), and `Ring::one()` / `Field::one()` is the multiplicative one. This generalises to structures with any number of operations.
- ~~`FiniteField(p)` accepted composite p~~ (now `std::invalid_argument`). Negative values of `Z/nZ` / `F_p` elements are reduced into range. `FiniteFieldElement::inverse(0)` throws `invalid_argument`.
- ~~`Matrix::operator*` returned a 0x0 matrix on a dimension mismatch~~; it now throws `std::invalid_argument`. `NumericAddition` checks lengths.
- Missing `override` keywords added across `abstract_algebra`.

Still open (need design decisions or analysis):
1. **SIMD paths never run on the dev machine (arm64).** They're AVX-only, so on Apple Silicon `vectorize=true` falls back to scalar; the benchmark compares scalar with itself (confirmed: identical timings). Needs NEON implementations (see "Linear algebra + SIMD").
2. **`Vector` is disconnected from `abstract_algebra`.** It used to inherit `VectorElement`. To reconnect it, decide how `Vector` relates to `VectorSpace` (e.g. `Vector` as the element type of a `RealVectorSpace<n>`, with the structure supplying the operations), since `VectorElement` is a CRTP template that needs a `StructureType`.
3. **`Field.hpp` <-> `FieldElement.hpp` include each other.** It compiles (include guards plus the order the files are included in), but it's fragile. Needs analysis: forward-declare, or split the example `FiniteField` types into their own header.
4. **Makefile is hand-maintained.** It works, but every new file needs a manual entry. Consider CMake (the machine has it) or wildcard rules. Note `make` here is Apple's GNU Make 3.81, so avoid newer GNU Make features.
5. **`Vector` compound ops (`+=`, `-=`) have no SIMD / `vectorize` option**, unlike `vector_add` / `vector_sub`.

## Next up

### 1. Symbolic core → calculus (current branch focus)
- [x] Evaluation environment: `evaluate(expr, interp, env)` / `evaluate_real(expr, env)`.
- [x] Missing nodes: Subtract, Divide, Negate (now operators in `Signature::standard()`).
- [x] `Operator` with arbitrary arity (`Arity::fixed` / `at_least`).
- [ ] Simplification pass (constant folding, `x*1`, `x+0`, `x^1`).
- [ ] Symbolic differentiation `d/dx` on the expression tree → fills `calculus/diff.hpp`.
- [ ] Use `Function`'s domain set: check that inputs lie in the domain on evaluate, and validate the domain of `Log`.

### 2. AST parser
- [x] Tokenizer + Pratt parser from string → `Expression` tree; round-trips `toString()`.
- [ ] Parse set notation (`[0, 1)`, `{1, 2}`, `A U B`, `A ∩ B`) into `Set` trees.

### 3. Sets
- [x] Dedicated `tests/sets/test_sets.cpp` doctest suite. Still to cover: `UnionSet`/`IntersectionSet` simplification and `complement()` of composites.
- [ ] Use sets as `Function` domain and codomain everywhere.

### 4. Linear algebra + SIMD
- [ ] Add ARM NEON paths to `Vector` (`float64x2_t`, `vaddq_f64`/`vsubq_f64`).
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
