# cppmath

A math library in C++, supporting numerous concepts I've learned as a math major :p

Alongside the math, it's a playground for low-level acceleration in C++: SIMD intrinsics, cache-friendly layouts, and templates, each benchmarked against a plain scalar version.

**What's in it:** abstract algebra (groups, rings, fields, vector spaces, Lie algebras), set theory, a symbolic expression core, linear algebra, numerics, number theory, combinatorics, and graph theory. See [PLAN.md](PLAN.md) for per-module status and the roadmap.

## Getting started

Uses git submodules (for [doctest](https://github.com/doctest/doctest)):

```sh
git clone --recurse-submodules <repo-url>
# or, if already cloned:
git submodule update --init --recursive
```

## Build & test

```sh
make test             # build and run all tests
make test_matrix      # build a single test, then ./test_matrix
make all              # tests + examples (./group_demo, ./set_demo, ...)
make run-benchmarks   # -O3 benchmarks, SIMD vs scalar
make clean
```

Without the Makefile:

```sh
g++ -std=c++17 -Iinclude -Iinclude/doctest/doctest -o test_graph tests/graph/test_graph.cpp src/graph/graph.cpp
./test_graph
```

## Docs

- [PLAN.md](PLAN.md): status, known issues, roadmap
- [CLAUDE.md](CLAUDE.md): repo conventions (layout, namespaces, SIMD and testing patterns)
- [docs/architecture/abstract_algebra.md](docs/architecture/abstract_algebra.md): algebraic structure class hierarchy
- [docs/architecture/sets.md](docs/architecture/sets.md): set class hierarchy
- [benchmarks/benchmarks.md](benchmarks/benchmarks.md): benchmark notes
