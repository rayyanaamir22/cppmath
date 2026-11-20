# geometry plan

_Last updated 2026-11-20 · status: **stub** · scope below is *proposed*_

Euclidean geometry in ℝⁿ: points, lines, planes, polygons, convex hulls and transformations, built on [linalg](../linalg/plan.md)'s vectors and matrices. A natural SIMD target (many points transformed at once), and later a source of n-dimensional regions for [sets](../sets/plan.md) and integration domains.

## Status

- `include/cppmath/geometry/geometry.hpp` and `src/geometry/geometry.cpp` are empty (comments only). No tests or Makefile rule.

## Roadmap *(proposed)*

- [ ] **1. Points and vectors.** `Point<N>` (fixed dimension, or dynamic over `linalg::Vector`, per the decision below) with distance, midpoint and vector differences. Add `tests/geometry/test_geometry.cpp` and Makefile rules.
- [ ] **2. Lines, segments and planes.** Parametric and implicit forms; point–line and point–plane distance; intersections (line–line in 2-D, line–plane in 3-D); projection.
- [ ] **3. Robust predicates.** Orientation (`orient2d`) and in-circle tests, with an explicit ε policy and a note on exact or adaptive arithmetic.
- [ ] **4. Polygons (2-D).** Shoelace area (signed), centroid, perimeter, point-in-polygon (winding number), convexity test.
- [ ] **5. Convex hull.** Andrew's monotone chain in 2-D (O(n log n)); test against brute force on random point sets.
- [ ] **6. Transformations.** Affine and homogeneous transforms as `linalg::Matrix` (rotation, scaling, translation, reflection), composition by matrix multiplication.
- [ ] **7. Later.** Triangulation (ear clipping), closest pair, segment-intersection sweep, 3-D convex hull.

## Performance

- Transform a batch of points by one matrix with SIMD (structure-of-arrays layout), against a scalar loop baseline; benchmark in `benchmarks/geometry/`.
- Data layout decision (array-of-structs vs struct-of-arrays) for point clouds, measured, not guessed.

## Connections

- **Needs:** [linalg](../linalg/plan.md) (vectors, matrices, decompositions for least-squares fitting), [utils](../utils/plan.md) (`approx_equal`, SIMD helper).
- **Offers:** n-D regions to [sets](../sets/plan.md) (products of intervals, half-spaces), domains to [calculus](../calculus/plan.md) integration, and examples for [stats](../stats/plan.md) (regression lines).

## Decisions needed

- **Dimension:** fixed at compile time (`Point<2>`, `Point<3>`, enabling `constexpr` and SIMD-friendly layouts) or dynamic (`linalg::Vector`)?
- **Floating-point robustness:** ε-based predicates (simple), or exact/adaptive predicates (correct)?
- **Symbolic geometry** (constructions as core expressions) in scope, or numeric only?
