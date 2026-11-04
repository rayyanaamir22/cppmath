# CLAUDE.md

Guidance for working in this repo. For what to build next and current status, see [PLAN.md](PLAN.md).

## What this is

`cppmath` is a personal C++17 math library that grows with the author's math-major coursework (abstract algebra, linear algebra, set theory, number theory, combinatorics, graph theory, calculus). It has two goals, and both matter:

1. **Model the mathematics faithfully.** Class hierarchies should mirror the math (a Field *is* an algebraic structure with two operations, a Set is something you can test membership in, etc.).
2. **Learn low-level acceleration.** Where it's a natural fit, use C++-specific performance features: SIMD intrinsics, cache-friendly layouts, templates and constexpr. Keep a plain scalar path next to each one so the two can be compared and benchmarked.

The code is a learning project as well as a library. When writing code, prefer clarity and explain the non-obvious parts (why an intrinsic is used, what invariant a class keeps), and don't use a clever abstraction where a simple one would do.

## Build and test

There is no CMake. Everything goes through the hand-written [Makefile](Makefile) with `g++ -std=c++17 -Iinclude -Iinclude/doctest/doctest`.

```sh
git submodule update --init --recursive   # doctest lives in include/doctest (submodule)
make test              # build + run every target in $(TESTS)
make test_prime        # build a single test binary, then ./test_prime
make all               # tests + examples
make benchmarks        # build benchmarks/*/*.cpp with -O3 (+ -mavx on x86_64)
make run-benchmarks
make clean
```

- **The Makefile lists targets by hand.** A new test, example, or `.cpp` dependency won't build until you add it to `TESTS`/`EXAMPLES` and give it a rule that lists every `src/*.cpp` it links against.
- Binaries go in the repo root (`./test_matrix`, etc.) and are gitignored.
- The dev machine is **Apple Silicon (arm64)**. The `#if defined(__x86_64__)` AVX paths are compiled out here and only the scalar fallback runs. See the SIMD section.

## Layout

Each module has its pieces at the same path under several roots:

```
include/cppmath/<module>/...hpp   public headers
src/<module>/...cpp               implementations
tests/<module>/test_*.cpp         doctest tests
examples/<topic>_demo.cpp         runnable demos (print to stdout)
benchmarks/<module>/bench_*.cpp   timing harnesses (std::chrono)
docs/architecture/<module>.md     design docs / mermaid diagrams
```

Modules: `abstract_algebra`, `core` (symbolic expressions), `sets`, `linalg`, `numerics`, `number_theory`, `combinatorics`, `graph`, plus placeholder stubs (`calculus`, `geometry`, `stats`, `algebra`, `utils`).

## Conventions

- **Namespaces:** `cppmath::<module>`, e.g. `cppmath::linalg`, `cppmath::abstract_algebra`, `cppmath::sets`, `cppmath::core`. Example or demo concrete types go in a nested `example` namespace (e.g. `abstract_algebra::example::FiniteField`).
- **Headers:** `#pragma once`. Include paths go from `include/`: `#include "cppmath/sets/Set.hpp"`. Prefer this over `../` relative includes; one of those is currently broken (see PLAN.md).
- **Templates:** template definitions live in the header (e.g. `Matrix<T>` is header-only; `src/linalg/matrix.cpp` is intentionally empty).
- **File naming:** class-per-file modules use `PascalCase.hpp` (`sets/`, `core/`, `abstract_algebra/`). Function-collection modules use `lowercase.hpp` (`prime.hpp`, `combo.hpp`, `root.hpp`).
- **Docs:** Doxygen-style `/** @brief ... */` on public APIs.
- **Errors:** throw `std::invalid_argument` or `std::out_of_range` on dimension mismatch, bad index, or bad domain.

### Abstract algebra (CRTP + virtual operations)

See [docs/architecture/abstract_algebra.md](docs/architecture/abstract_algebra.md).
- Structures are `template<typename Derived, typename ElementType>` (CRTP) and inherit `AlgebraicStructure<ElementType>`.
- Each structure has a matching element type that inherits `AlgebraicElement<Derived, StructureType>`.
- Operations are `AlgebraicOperation<ElementType, StructureType>` subclasses, exposed through pure virtual accessors (`operation()`, `addition_op()`, `multiplication_op()`, `scalar_multiplication_op()`).
- Concrete finite structures (e.g. `FiniteField` mod p, S3) are examples. They are not part of the abstract hierarchy.

### Sets and symbolic core (immutable trees of `unique_ptr`)

See [docs/architecture/sets.md](docs/architecture/sets.md).
- `sets::Set` and `core::Expression` are abstract bases manipulated polymorphically through `std::unique_ptr`. Operations return **new** objects and never mutate their inputs. Every node implements `clone()` and `toString()`.
- `toString()` output is meant to be **parseable by a future AST parser**. Keep it canonical and fully parenthesised (`(x + 1)`, `([0, 1) U {1, 2})`).
- `core::Function` holds non-owning `const sets::Set*` for domain and codomain, and owns its `rule` expression.

### SIMD

The reference implementation is `linalg::Vector` in [src/linalg/vector.cpp](src/linalg/vector.cpp):
- A public method takes a `bool vectorize` flag. The SIMD path runs only if the flag is set **and** the platform macro is defined. A scalar tail loop handles the leftover elements.
- Guard with `#if defined(__x86_64__) || defined(_M_X64)` / `#include <immintrin.h>`. Always keep a scalar `#else` fallback.
- Use unaligned loads and stores (`_mm256_loadu_pd`) unless the storage is known to be aligned.
- Each SIMD path should have a test that checks it matches the scalar path, and a benchmark in `benchmarks/<module>/` that times both.
- Because development happens on arm64, new SIMD work should add an ARM NEON path (`#if defined(__ARM_NEON)` / `<arm_neon.h>`, `float64x2_t`) alongside AVX. Otherwise the vector path never runs locally.

## Testing

- Tests use **doctest**: `#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN`, `#include "doctest.h"`, `TEST_CASE` / `SUBCASE` / `CHECK`. Compare floats with `doctest::Approx`.
- `tests/core/test_expression.cpp` (and its identical twin `test_core.cpp`) is a print-only smoke test with no assertions. It needs to be converted to doctest (tracked in PLAN.md).
- Run the relevant test target after any change, and `make test` before committing.

## Git

- Work happens on feature branches named after the module (`abstract-algebra`, `calculus`, `core`) and merges to `main` via PR.
