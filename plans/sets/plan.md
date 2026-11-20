# sets plan

_Last updated 2026-11-20 · status: **active**_

Set theory over the real line: intervals, finite sets, unions, intersections, complements, the empty set and ℝ, as immutable trees. Sets are the domains and codomains of `core::Function`. Through [core](../core/plan.md) they can be written as text (`([0, 1) U {1, 2})`), parsed and evaluated.

How it is built (class hierarchy, immutability, text format): [docs/architecture/sets.md](../../docs/architecture/sets.md).

## Status

- `IntervalSet`, `FiniteSet`, `UnionSet`, `IntersectionSet`, `EmptySet`, `UniversalSet` behind the abstract `Set`.
- Operations return new sets; `toString()` text parses through core. Numbers print with `utils::format_number`.
- `FiniteSet` is immutable through `with_element` / `without_element`.
- Tolerance: 1e-9 for membership and equality.
- Tests: `test_sets` (15 cases), plus `test_setexpr` in core (round-trips of this module's text). Example: `set_demo`.

**Known limits**
- **Structural equality:** `equals` is structural, not set-theoretic: `{1}` ≠ `[1, 1]`, and `[0, 1] U [2, 3]` built two ways may differ.
- **No normal form:** unions and intersections don't normalise, so composite sets grow (complements of unions nest deeply).
- **Reals only:** sets contain reals only (`contains(double)`); there are no sets of vectors, field elements or pairs.
- **Mutable field:** `FiniteSet::elements` is a public, mutable member, even though the class is meant to be immutable.
- **Duplication:** each `.cpp` has its own copy of `double_eq` / `EPSILON`.
- **Gaps:** `complement()` of composite sets, and the simplification inside `UnionSet`/`IntersectionSet`, are not covered by `test_sets`.
- **Stub:** `sets.hpp` / `sets.cpp` are empty.

## Roadmap

- [ ] **1. A normal form for subsets of ℝ.** Represent any finite union of intervals and points as a sorted list of disjoint, non-touching intervals (points are degenerate closed intervals).
  - `normalize(set)` turns any composite set into it.
  - Set-theoretic equality becomes decidable (`equivalent(a, b)`), and complements stay flat.
  - *Tests:* equivalence of differently-built equal sets; complement of complement; De Morgan on random sets.
- [ ] **2. Missing operations:** difference `A \ B`, symmetric difference, subset `A ⊆ B` and proper subset, disjointness. The core operators for them come in [core](../core/plan.md) item 6.
- [ ] **3. Measure and size:** Lebesgue measure (total length) of a normalised set, cardinality of finite sets, and `is_finite` / `is_bounded` / `inf` / `sup`.
- [ ] **4. Encapsulation and clean-up:** make `FiniteSet::elements` private with a read-only accessor; replace the `double_eq` copies with `utils::approx_equal` ([utils](../utils/plan.md)); fill or delete `sets.hpp`/`sets.cpp`; add the missing tests.
- [ ] **5. Generic elements (design):** sets of something other than reals: field elements, vectors, pairs. Template `Set<T>`, or sets of core `Value`s?
- [ ] **6. Products and n-D regions:** Cartesian products of intervals (boxes), half-spaces, for [geometry](../geometry/plan.md) and integration domains in [calculus](../calculus/plan.md).
- [ ] **7. Functions on parsed domains:** `core::Function` taking its domain and codomain as parsed set expressions, with membership checked through them.

## Performance

- Membership in a normalised set is a binary search over sorted intervals (O(log n)), instead of walking a tree of unions.

## Connections

- **Needs:** [utils](../utils/plan.md) (`format_number`, `approx_equal`).
- **Offers:** domains and codomains to [core](../core/plan.md) `Function`; set values to core's `Value`; supports of distributions to [stats](../stats/plan.md); regions to [geometry](../geometry/plan.md) and [calculus](../calculus/plan.md).

## Decisions needed

- **`equals`:** should it become set-theoretic equality once the normal form exists, or stay structural with a separate `equivalent`?
- **Tolerance:** keep a fixed 1e-9, or make it configurable or relative?
- **Generic element type:** `Set<T>` templates (a big refactor) or `Value`-based sets?
