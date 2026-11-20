# core plan

_Last updated 2026-11-20 · status: **active** · all 7 original milestones done_

`core` is the syntax layer of cppmath, and the centre of the symbolic engine. It holds immutable expression trees over a vocabulary of operators, a parser and printer for them, and pluggable interpretations that give a tree a value. Calculus, simplification, set expressions and algebraic evaluation are all *consumers* of it.

How it is built (layers, contracts, the standard vocabulary, design decisions): [docs/architecture/core.md](../../docs/architecture/core.md).

## Status

| # | Milestone | State |
|---|---|---|
| 1 | Syntax metadata: `Sort`, `Operator`, `Signature`, `standard()` | done |
| 2 | Generic terms: `Expression`, `Constant`, `Variable`, `Apply`, builders | done |
| 3 | Traversal + semantics: `fold`, `transform`, `Substitution`, `Interpretation`, `Environment`, `evaluate`, `Function` | done |
| 4 | Parser: `Lexer`, Pratt `Parser`, `ParseError`, canonical printer, round-trip tests | done |
| 5 | Sets through core: relations, logic, set operators, interval/set literals, the multi-sorted `Value` | done, except general overload resolution (deferred: roadmap item 1) |
| 6 | Binders: `Bind`, scoped evaluation, capture-avoiding substitution, `sum`/`prod` | done (λ, ∀/∃, ∫, set-builder not built: roadmap item 3) |
| 7 | Second carrier: F_p interpretation, `examples/core_demo.cpp` | done |
| + | `core::simplify`; `calculus::differentiate` (in [calculus](../calculus/plan.md)) | done |

| Test target | Cases | Assertions | Covers |
|---|---|---|---|
| `test_signature` | 10 | 65 | sorts, lookups, operator shapes (incl. binders), variadics, `extend`, reserved words |
| `test_expression` | 10 | 51 | `Constant`/`Variable` invariants, builders, `Apply` validation, clone/equals/hash, `Function` |
| `test_traversal` | 10 | 25 | `fold`, `transform`, analysis, `Substitution` (simultaneous, sort-checked, `compose`) |
| `test_evaluate` | 8 | 37 | environments, real evaluation, domain errors, a second carrier (Z/7) |
| `test_binders` | 29 | 149 | `Bind`, free variables, α-equivalence, capture-avoiding substitution, `sum`/`prod`, parsing |
| `test_simplify` | 11 | 227 | identities, signs, exact folding, idempotence, value preservation |
| `test_setexpr` | 40 | 492 | `Value`, the sets/logic vocabulary, brackets, aliases, declared sorts, evaluation, round-trips with the `sets` library's text |
| `test_parser` | 17 | 119 | lexer, precedence, associativity, calls, variadics, error positions, round-trips |

Consumers with their own tests: `test_field_interpretation` (10 cases, [abstract_algebra](../abstract_algebra/plan.md)) and `test_diff` (11 cases, [calculus](../calculus/plan.md)).

## Roadmap

Ordered so each item unlocks the next. Items marked **design** need a decision (see [Decisions needed](#decisions-needed)) before code.

- [ ] **1. Overload resolution (design).** Today `=`/`!=` are the only sort-polymorphic operators (`Any × Any → Bool`, checked at evaluation). Matrix `*`, vector `+` and a statically-checked `=` need several `Operator`s per (notation, symbol) and a sort-resolution pass after parsing that picks the unique overload from the declared variable sorts.
  - *Files:* `Signature.{hpp,cpp}` (multimap by symbol), a new `Resolve.{hpp,cpp}` (or a parser stage), `Parser.cpp`, `Printer.cpp` (unchanged text).
  - *Tests:* a new `test_overloads`: resolution, ambiguity and no-match errors with positions, round-trips.
  - *Done when:* `=` is checked at construction, and a test signature with `*` on two sorts parses and evaluates both.
- [ ] **2. A linalg interpretation (design; needs 1).** A carrier that extends `Value` with vectors and matrices, `Vector`/`Matrix` sorts, matrix literals (`[[1, 2], [3, 4]]` versus the interval `[a, b)`), and an `Interpretation` over `linalg::Matrix<double>`, living in linalg the way `FieldInterpretation` lives in abstract_algebra. Owned jointly with the [linalg plan](../linalg/plan.md).
- [ ] **3. More binders (design).** They reuse `Bind`, `define_binder` and capture-avoiding substitution.
  - λ (functions as values).
  - ∀ / ∃ with Bool bodies, decidable by enumeration over finite sets only. Over an interval they must say so.
  - ∫, `integral(x, a, b, f)`, for [calculus](../calculus/plan.md).
  - Set-builder `{x ∈ A | P}`, which needs a `|` token and a Bool-bodied binder that produces a Set.
- [ ] **4. Conditionals.** `if(cond, a, b)` and piecewise expressions, with lazy (short-circuit) evaluation through a hook like `define_binder`'s body callback. The derivative goes in calculus.
- [ ] **5. Number systems (design).** `Integer`/`Rational` literal sorts with exact arithmetic (so `simplify` folds `1/3` exactly), shared with [number_theory](../number_theory/plan.md). Complex numbers later.
- [ ] **6. Sets vocabulary.** Difference `\`, subset `⊆`, Cartesian product, cardinality. The semantics come from the [sets plan](../sets/plan.md); core adds the operators, aliases and parser support.
- [ ] **7. Simplification.** Like terms and `x*x → x^2` through the [algebra](../algebra/plan.md) polynomial normal form, `x/x`, Boolean and set identities (`A U ∅ = A`, `not not p`). Also pretty-printing with minimal parentheses.
- [ ] **8. Equality and identity.** `Bind::equals` as α-equivalence, with `hash` to match. Hash-consing only if a benchmark asks for it.
- [ ] **9. Parser polish.** Implicit multiplication (`2x`), UTF-8 identifiers, `Notation::Postfix` (for `n!`, see [combinatorics](../combinatorics/plan.md)).
- [ ] **10. Category theory (exploratory).** `Category`/`Functor`/natural-transformation concepts in a future `cppmath::category`: `Signature` ⇒ theory, `Interpretation` ⇒ model, `fold` ⇒ the induced homomorphism (see the mathematical-model table in the architecture doc). It gets its own `plans/category/` once it has code.

## Performance

- Vectorised evaluation of one expression over many inputs (compile the tree to a flat instruction list, then evaluate lanes with SIMD), shared with [numerics](../numerics/plan.md). Scalar `evaluate` is the baseline; add a benchmark under `benchmarks/core/`.
- No hash-consing, tree sharing or expression templates until a benchmark shows they are needed.

## Connections

- **Offers:** the AST, parser, printer and interpretations to [calculus](../calculus/plan.md), [sets](../sets/plan.md) (set text), [abstract_algebra](../abstract_algebra/plan.md) (F_p and other carriers), [algebra](../algebra/plan.md) (polynomials ↔ expressions) and [numerics](../numerics/plan.md) (root finding on expressions).
- **Needs:** `sets` (the only module core depends on), `utils::format_number`.
- **Dependency rule:** other modules depend on core, never the reverse (except `sets`).

## Decisions needed

- **Overload resolution:** bottom-up sort inference, or unification with sort variables? How to report ambiguity? Does `Any` stay?
- **Extending `Value`:** keep a closed union and add `Vector`/`Matrix`, or move to an open registry of carriers per `Sort` once linalg needs it?
- **Matrix literals:** `[[1, 2], [3, 4]]` has to coexist with the intervals `[a, b)`/`[a, b]`. A rule such as "`[` followed by `[` is a matrix" is the obvious candidate.
- **∀/∃ over infinite sets:** refuse, or allow only over finite and finitely-representable sets?

## Open questions

- Exact numerics: `Integer`/`Rational` literal sorts versus `double` only.
- Whether `Sort` should eventually *be* a `sets::Set` (dependent typing like `x : [0, 1)`) or stay a pure tag.
- Implicit multiplication (`2x`) in the parser.
- Whether `Bind::equals` should become α-equivalence.
- Whether `and`/`or` should short-circuit (ties to roadmap item 4).
