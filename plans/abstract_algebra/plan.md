# abstract_algebra plan

_Last updated 2026-11-20 · status: **active**_

Models algebraic structures the way the coursework defines them. A structure is a carrier set together with operations: groups, rings, fields, vector spaces and Lie algebras, each with its element type. It is the "faithful mathematics" pillar of cppmath, and the place where concrete carriers for core's interpretations come from (F_p today).

How it is built (CRTP hierarchy, operations as objects): [docs/architecture/abstract_algebra.md](../../docs/architecture/abstract_algebra.md).

## Status

- **Abstract layer:** `AlgebraicStructure`, `AlgebraicElement`, `AlgebraicOperation`; CRTP `Group`, `Ring`, `Field`, `VectorSpace`, `Lie` and their element types.
- **Identities:** each identity lives on its operation (`has_identity()`, `identity(structure)`). `identity()` is the additive/primary one; `Ring::one()` / `Field::one()` give the multiplicative one.
- **Example types** (namespace `example`): `SymmetricGroup3`, `IntegerMod4`, `FiniteField` (prime moduli only), `R2VectorSpace`, `NumericVectorSpace`, `HeisenbergLie`.
- **Core interop:** `finite_field_interpretation` in `field/FieldInterpretation.hpp` evaluates core expressions in F_p.
- **Tests:** `test_finite_field`, `test_structures`, `test_field_interpretation`. Examples: `field_demo`, `group_demo`, `ring_demo`, `vsp_demo`, `lie_demo`.

**Known issues**
- `field/Field.hpp` and `field/FieldElement.hpp` include each other. It compiles only because of include guards and include order.
- `AlgebraicElement::value()` returns `int`, which bakes "elements are integers" into the base. Permutations, vectors and Lie elements return a meaningless 0.
- Example structures hand out elements whose parent is a *copy* of the structure (`make_shared(*this)`), not the structure itself.
- `src/abstract_algebra/**.cpp` include headers through `../../../include/...` relative paths, against the CLAUDE.md include rule.
- `*Element.cpp` files contain only a comment.
- `linalg::Vector` is disconnected from `VectorSpace`. This is shared with [linalg](../linalg/plan.md): it used to inherit `VectorElement`.

## Roadmap

- [ ] **1. Clean-up.**
  - Switch to `#include "cppmath/..."`.
  - Break the `Field.hpp` ↔ `FieldElement.hpp` cycle by forward-declaring, and move each `example::` type into its own header (`field/FiniteField.hpp`, ...).
  - Delete or fill the empty `*Element.cpp`.
  - Replace `int value()` with a per-type representation (decision below).
  - Hand out elements whose parent is the real structure (`enable_shared_from_this`, or structures always held by `shared_ptr`).
  - *Done when:* every header compiles on its own (`clang++ -fsyntax-only` per header) and the tests pass unchanged.
- [ ] **2. Polynomial rings.** `PolynomialRing<R>`: a `Ring` whose elements are [algebra](../algebra/plan.md)'s `Polynomial<T>` over a ring or field R. Tests: the ring axioms on samples, and division with remainder over a field.
- [ ] **3. Quotient structures.**
  - Z/nZ as a quotient of Z, replacing the hard-coded `IntegerMod4` with `IntegerMod<n>` or a runtime n.
  - Quotient groups G/N for a normal subgroup N.
  - F_p[x]/(f) for an irreducible f, giving the finite fields GF(pⁿ). Uses item 2 and [number_theory](../number_theory/plan.md) for irreducibility tests.
- [ ] **4. General symmetric groups Sₙ.** Generalise `SymmetricGroup3`, using [combinatorics](../combinatorics/plan.md)'s permutation generation; cycle notation, sign and order of an element.
- [ ] **5. Modules over rings,** generalising `VectorSpace` from fields to rings.
- [ ] **6. Field extensions and Galois groups:** degree, minimal polynomials, automorphisms of small extensions (exploratory).
- [ ] **7. Representations:** group homomorphisms into `linalg::Matrix` (permutation and regular representations), characters later. Needs [linalg](../linalg/plan.md) matrix basics.
- [ ] **8. More core interpretations:** Z/nZ (a ring, so `div` is undefined where there is no inverse) and group words (`a * b^-1`) evaluated in a finite group.
- [ ] **9. Reconnect `linalg::Vector` and `VectorSpace`:** for example, `Vector` as the element type of a `RealVectorSpace<n>` whose structure supplies the operations. Decided together with linalg.
- [ ] **10. Category theory (exploratory):** structures as objects and homomorphisms as morphisms. Ties to core's Signature/Interpretation view; see the [core plan](../core/plan.md) item 10.

## Performance

- **Cayley tables** for finite structures: precompute each operation as a flat `n × n` table indexed by element ordinals, making composition an O(1) lookup. Benchmark against computing the operation (`benchmarks/abstract_algebra/bench_cayley.cpp`), for S₅ and GF(p) of various sizes.
- **Batched modular arithmetic** in F_p with SIMD (vectors of residues), with the scalar loop as baseline.

## Connections

- **Needs:** [algebra](../algebra/plan.md) (polynomials), [number_theory](../number_theory/plan.md) (primality, modular inverse; `FiniteField`'s private `checked_prime` and extended Euclid should come from there), [combinatorics](../combinatorics/plan.md) (permutations), [linalg](../linalg/plan.md) (representations).
- **Offers:** carriers and interpretations to [core](../core/plan.md).

## Decisions needed

- **Element representation:** replace `int value()` with what? A template value type, or no common accessor at all?
- **Library vs examples:** which structures graduate from `example::` into the library proper (Z/nZ, GF(pⁿ), Sₙ)?
- **Dependency direction with number_theory:** `abstract_algebra` → `number_theory` seems natural (fields need primality and inverses).
- **Rings vs fields:** should `Field` derive from `Ring` (it doesn't today), so field code can be used where a ring is expected?
