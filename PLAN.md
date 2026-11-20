# cppmath Plan

cppmath is a personal C++17 math library that grows with a math-major's coursework, and doubles as a project for learning low-level acceleration. This file states what the project is for and how the work fits together. What to do in each module lives in [`plans/<module>/plan.md`](plans/). Build instructions and conventions are in [CLAUDE.md](CLAUDE.md); an overview is in [README.md](README.md).

_Last reviewed: 2026-11-20._

## Central goals

1. **Model the mathematics faithfully.** Class hierarchies mirror the definitions from the coursework (a field *is* a ring-like structure with two operations; a set is something you can test membership in). Correctness and the right abstraction come before features.
2. **A symbolic engine: parse → manipulate → solve.** [core](plans/core/plan.md)'s expression trees are the centre:
   - text is parsed into a tree;
   - calculus, simplification and substitution transform it;
   - interpretations evaluate it in any algebra (ℝ, F_p, sets, and soon matrices);
   - the end goal is solving: equations, systems and integrals, symbolically where possible and numerically otherwise.
3. **Low-level acceleration.** SIMD intrinsics (AVX and NEON), cache-friendly layouts, `constexpr`. Every fast path sits next to a plain scalar path, with a test that they agree and a benchmark that shows the difference.
4. **A learning project.** Code explains its non-obvious parts, public APIs are documented (shown on hover), and simple abstractions beat clever ones.

## Modules

| Module | What it is | Status | Plan | How it's built |
|---|---|---|---|---|
| `core` | symbolic expressions: AST, parser, printer, interpretations, substitution, simplify | active | [plan](plans/core/plan.md) | [architecture](docs/architecture/core.md) |
| `calculus` | differentiation; integration, limits, series next | active | [plan](plans/calculus/plan.md) | (see core's doc) |
| `sets` | subsets of ℝ: intervals, finite sets, unions, intersections | active | [plan](plans/sets/plan.md) | [architecture](docs/architecture/sets.md) |
| `abstract_algebra` | groups, rings, fields, vector spaces, Lie algebras | active | [plan](plans/abstract_algebra/plan.md) | [architecture](docs/architecture/abstract_algebra.md) |
| `linalg` | vectors, matrices, decompositions; the SIMD reference module | active | [plan](plans/linalg/plan.md) | n/a |
| `algebra` | concrete polynomials | stub | [plan](plans/algebra/plan.md) | n/a |
| `number_theory` | primes, gcd, modular arithmetic | early | [plan](plans/number_theory/plan.md) | n/a |
| `combinatorics` | counting and enumeration | early | [plan](plans/combinatorics/plan.md) | n/a |
| `numerics` | root finding, quadrature, ODEs | early | [plan](plans/numerics/plan.md) | n/a |
| `graph` | graphs, traversal, paths, spectra | early | [plan](plans/graph/plan.md) | n/a |
| `geometry` | points, polygons, hulls, transforms in ℝⁿ | stub | [plan](plans/geometry/plan.md) | n/a |
| `stats` | descriptive statistics, distributions, regression | stub | [plan](plans/stats/plan.md) | n/a |
| `utils` | shared helpers (number formatting; SIMD, comparison, benchmarking next) | support | [plan](plans/utils/plan.md) | n/a |

_Active_ means real code and tests with a roadmap in progress. _Early_ means some functions exist, with known gaps or bugs listed in the plan. _Stub_ means an empty header and source, with a proposed scope.

## Cross-module tracks

Work that spans modules, in priority order. Each step names the plan that owns it; tick items there, not here.

### 1. Symbolic engine (first priority)
1. **Overload resolution** in core: several operators per symbol, chosen by sort ([core](plans/core/plan.md) item 1).
2. **A matrix interpretation:** matrices and vectors as expression values ([core](plans/core/plan.md) item 2, [linalg](plans/linalg/plan.md) item 7).
3. **Calculus:** partial derivatives, Taylor polynomials, then integration with a numerical fallback ([calculus](plans/calculus/plan.md) items 1–5, [numerics](plans/numerics/plan.md) item 2).
4. **Polynomials:** `Polynomial<T>` and conversion to and from expressions, which also gives `simplify` like-term collection ([algebra](plans/algebra/plan.md) items 2–5).
5. **Solving:** `solve(equation, x)` that dispatches by shape:
   - linear systems → [linalg](plans/linalg/plan.md) decompositions;
   - polynomial equations → [algebra](plans/algebra/plan.md) roots;
   - anything else → [numerics](plans/numerics/plan.md) root finding on the expression.
   It returns exact solutions where it can and numerical ones (with tolerance) otherwise. Where `solve` lives (core consumer, or a new `solve` module) is decided when it starts.

### 2. SIMD and performance
1. **The shared helper** `utils/simd.hpp` with AVX and NEON ([utils](plans/utils/plan.md) item 1), so SIMD finally runs on the arm64 dev machine.
2. **linalg kernels:** NEON add/sub, FMA dot product, blocked and SIMD matrix multiply ([linalg](plans/linalg/plan.md) items 1–4).
3. **SIMD reductions** in [stats](plans/stats/plan.md) (item 2).
4. **Batch evaluation** of expressions over many points ([numerics](plans/numerics/plan.md), [core](plans/core/plan.md)).
5. **A benchmark harness** and recorded results ([utils](plans/utils/plan.md) item 3, [benchmarks/benchmarks.md](benchmarks/benchmarks.md)).
6. **`constexpr` tables** ([combinatorics](plans/combinatorics/plan.md), [number_theory](plans/number_theory/plan.md)) and Cayley tables ([abstract_algebra](plans/abstract_algebra/plan.md)), each benchmarked.

### 3. Abstract algebra
Clean-up → polynomial rings (needs [algebra](plans/algebra/plan.md)) → quotient structures and GF(pⁿ) → Sₙ → modules → Galois theory → representations (needs [linalg](plans/linalg/plan.md)). See [abstract_algebra](plans/abstract_algebra/plan.md). Category theory is exploratory and shared with core.

### 4. Correctness backlog
Bugs found while writing the plans, each listed in its module's plan:
- **number_theory:** `gcd`, `lcm` and `next_prime` are unimplemented, and the tests assert that they throw.
- **graph:** dangling `GraphNode` pointers; `is_tree` is unimplemented.
- **combinatorics:** `binomial_coefficient` overflows past 20!.
- **Error types:** several modules use `std::runtime_error` against the convention.
- **abstract_algebra:** relative includes and a header cycle.

These are small, and worth clearing before building on those modules.

### 5. Infrastructure
- **Build system.** The Makefile is hand-maintained: every new test or `.cpp` needs a manual rule. Options: wildcard rules (`core` already uses `$(wildcard ...)`), or CMake (installed on the dev machine). `make` here is Apple's GNU Make 3.81, so avoid newer GNU Make features.
- **Documentation upkeep:** the CLAUDE.md documentation rules (Doxygen on every public API, `-Wdocumentation` clean) and the plan/doc split below.

## How plans work

- **One plan per module:** `plans/<module>/plan.md`, mirroring `src/<module>` and `include/cppmath/<module>`.
- **Same shape everywhere:** Status (what exists, known bugs) → Roadmap (ordered checkboxes; each with the files it touches, its tests, and when it's done) → Performance → Connections → Decisions needed.
- **Proposed scope** in early modules is marked *proposed*: trim it freely.
- **Plans say what to do; [`docs/architecture/<module>.md`](docs/architecture/) says how it is built.** When a roadmap item is finished, tick it in the plan and move any lasting "how it works" explanation into the architecture doc (create one when a module's design settles).
- This file holds only goals, the module index and cross-module tracks. Resolved work is in git history.

## Long-term vision

A single symbolic mathematics engine. You type an expression or equation, then:
- it is parsed into a tree;
- it is manipulated (simplified, differentiated, integrated, substituted);
- it is solved;
- it is evaluated over whatever structure it lives in: real numbers, finite fields, sets, matrices, polynomial rings.

Abstract algebra supplies the structures, linear algebra and numerics the fast numerical machinery, and core's signature/interpretation design keeps them connected, with room for a category-theoretic view of the whole.
