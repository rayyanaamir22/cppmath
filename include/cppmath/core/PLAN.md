# `core` framework plan

## Goal
`core` is the **syntax layer** of cppmath. It represents mathematical terms as immutable trees and gives each tree meaning through pluggable interpretations. It also reads and prints terms losslessly. Everything else (calculus, simplification, set parsing, algebraic evaluation) is a *consumer* of core. Nothing in core should know about `double`, `sin`, or ℝ except through a registered `Signature` and `Interpretation`.

Design rule: **separate syntax from semantics.**
- *Syntax* (sorts, operators, signatures, terms, parser, printer) knows only shapes.
- *Semantics* (interpretations, environments, evaluation) maps shapes into a carrier type.

## Mathematical model (why the classes look like this)
| Math | Class | Category-theory reading |
|---|---|---|
| Sort (type of a term: Real, Bool, Set, ...) | `Sort` | object |
| Operation symbol `f : s₁ × … × sₙ → s` | `Operator` | generating (multi)morphism |
| Many-sorted signature Σ | `Signature` | presentation of a (multi)category / Lawvere theory |
| Terms over Σ with variables X | `Expression` (`Constant`, `Variable`, `Apply`, later `Bind`) | free Σ-algebra T_Σ(X) |
| Σ-algebra with carrier A | `Interpretation<A>` | algebra for the signature functor |
| Variable assignment X → A | `Environment<A>` | arrow out of the generators |
| Evaluation | `fold` / `evaluate` | unique homomorphism T_Σ(X) → A (initiality) |
| Substitution X → T_Σ(Y) | `Substitution` | Kleisli arrow of the term monad |

The rest of the design keeps this table true. Later, a `cppmath::category` namespace can define `Category`, `Functor`, and `Morphism` and treat these classes as instances without rewriting core.

## Components

### Layer 0: Sorts and operators (syntax metadata)
- **`Sort`** (`Sort.hpp`): a lightweight named type tag (`Real`, `Integer`, `Bool`, `Set`, `Any`) compared by identity. It is not an enum, so new sorts (`Matrix`, `Group`, ...) can be added without editing core.
- **`Operator`** (`Operator.hpp`): an immutable descriptor holding
  - `name` (canonical, e.g. `"add"`) and `symbol` (printed token, e.g. `"+"`)
  - `Arity` (fixed n, or variadic ≥ n)
  - `signature`: input sorts → output sort
  - `Notation`: `Infix | Prefix | Postfix | FunctionCall | Bracket`
  - `precedence` and `Associativity` (`Left | Right | None`), which the Pratt parser and printer use
  - `Properties` flags: associative, commutative, has identity. These are *syntactic hints* for simplification and mirror `abstract_algebra::AlgebraicOperation::is_associative`/`is_commutative`, but don't depend on that module.

  An `Operator` has **no evaluation code**. Meaning comes from interpretations, and derivatives come from a separate table in calculus.
- **`Signature`** (`Signature.hpp`): owns `Operator`s and looks them up by name and by symbol/notation (for the lexer and parser). It can be extended or composed (`Signature::extend(other)`). `Signature::standard()` returns a function-local static containing arithmetic (`+ - * / ^`, unary `-`), elementary functions (`sin cos exp log`, `sqrt`, ...), relations (`= < ≤`), logic (`∧ ∨ ¬`), and set operators (`U ∩`, complement, `∈`).
  - Ownership invariant: an `Expression` stores a non-owning `const Operator*`, so the `Signature` must outlive every expression built from it. Document this the same way `Function` documents its domain pointers.

### Layer 1: Terms (the AST)
- **`Expression`** (abstract): `sort()`, `children()` (a read-only view), `clone()`, `equals(const Expression&)` (structural), `hash()`, `accept(ExpressionVisitor&)`. It is immutable and owned through `std::unique_ptr`, as before.
- **Leaves:**
  - `Constant`: a numeric literal (`double` now). It keeps the source lexeme, so `0.1` prints back as `0.1`. Exact `Integer`/`Rational` literals are left for later.
  - `NamedConstant`: `π`, `e`, `∅`, `ℝ`. This is a nullary `Operator` from the signature, so it is really `Apply` with zero children plus a convenience factory.
  - `Variable`: a name plus a `Sort`.
- **`Apply`**: a `const Operator*` plus `std::vector<std::unique_ptr<Expression>>`. The constructor checks arity and child sorts, and throws `std::invalid_argument` on a mismatch.
- **`Bind`** (phase 4): binder operator + bound `Variable`(s) + body (+ optional bounds). It covers λ, ∀/∃, Σ/∏, ∫, and set-builder `{x ∈ A | P}`. Introducing it is what makes substitution capture-avoiding.
- **`Builders.hpp`**: factory functions `add(a, b)`, `mul`, `pow`, `neg`, `sin(a)`, `var("x")`, `num(3)`, ... over `Signature::standard()`. These replace `AddExpression`, `SinExpression`, and the rest. `BinaryOperations.*` and `UnaryOperations.*` are deleted.

### Layer 2: Traversal
- **`ExpressionVisitor`** (`Visitor.hpp`): one `visit` per node kind (`Constant`, `Variable`, `Apply`, later `Bind`). There are few node kinds, so the visitor stays small and does not grow with each new operator.
- **`fold<R>`** (`Fold.hpp`, header-only template): a generic catamorphism. It takes leaf handlers and an `Apply` handler `(const Operator&, std::vector<R>) -> R`. Evaluation, printing, free-variable collection, depth, and size are all folds.
- **`transform`**: a bottom-up rewrite (`Expression → unique_ptr<Expression>`), which is the basis for simplification and differentiation.
- **`Substitution`**: a map from `Variable` to `Expression`, with `apply(expr)` and `compose(σ, τ)`. Capture-avoiding once `Bind` exists.
- Utilities: `free_variables(expr)`, `contains(expr, var)`, `size`, `depth`.

### Layer 3: Semantics
- **`Interpretation<Carrier>`** (template, header-only): maps `const Operator*` to `std::function<Carrier(const std::vector<Carrier>&)>`, plus a literal → Carrier conversion. It throws if an operator has no meaning in this carrier.
- **`Environment<Carrier>`**: variable name → value bindings. It can be layered: child scopes for binders.
- **`evaluate(expr, interp, env)`**: the fold into `Carrier`. It replaces `Expression::evaluate()`.
- Provided interpretations:
  - `real_interpretation()` → `double` (current behaviour; `Log` checks its domain)
  - later: `FiniteField` elements (`abstract_algebra::example`), `linalg::Matrix<T>`, and `sets::Set` (so a parsed `([0, 1) U {1, 2})` evaluates to a real `UnionSet` through the existing `Set::set_union` / `intersect`)

  These show one AST serving many algebras.
- **`Function`** (reworked): name, parameter `Variable`s, `const sets::Set*` domain/codomain (non-owning, as now), owned rule. `operator()(args, interp)` checks domain membership, then evaluates.

### Layer 4: Concrete syntax (parser support), `core/parse/`
- **`SourceSpan`**: offset, line, and column, for error messages.
- **`Token` / `TokenKind`**: number, identifier, operator symbol, `( ) [ ] { } , |`, end-of-input.
- **`Lexer`**: longest-match on the symbols registered in the `Signature`, so new operators need no lexer edits. It reads UTF-8 for `∩ ∅ ℝ ≤ ∈`, and accepts ASCII aliases where they exist.
- **`ParseError`**: derives from `std::invalid_argument` and carries a `SourceSpan` plus what was expected.
- **`Parser`**: a Pratt parser whose binding powers come from `Operator::precedence`/`Associativity`. It also handles function-call syntax `f(a, b)` and bracket notation. The known ambiguity: `[a, b)` / `(a, b]` intervals versus grouping parentheses. The fix is to treat `(` followed by `expr , expr` and then `]`/`)` as an interval literal, and to test it explicitly.
- **`Printer`**: a fold driven by `Notation`. Canonical mode stays fully parenthesised, exactly matching current output (`(x + 1)`, `sin(x)`, `(2 ^ 3)`); a pretty mode (minimal parentheses) comes later.
- **Round-trip invariant (tested):** `parse(print(e)).equals(e)` for every `e`, and `print(parse(s)) == canonical(s)`.

### Out of core, but enabled by it
- `calculus::differentiate`: a per-`Operator` derivative rule table plus `transform` (fills `calculus/diff.hpp`).
- `core::simplify`: rewrite rules (constant folding, `x+0`, `x*1`, `x^1`) that use the `Properties` flags.
- `cppmath::category`: `Category`/`Functor`/natural transformation concepts. `Signature` ⇒ theory, `Interpretation` ⇒ model, `fold` ⇒ the induced homomorphism.

## Header layout
```
include/cppmath/core/
  Sort.hpp  Operator.hpp  Signature.hpp
  Expression.hpp  Constant.hpp  Variable.hpp  Apply.hpp  (Bind.hpp later)
  Builders.hpp  Visitor.hpp  Fold.hpp  Substitution.hpp
  Interpretation.hpp  Environment.hpp  Evaluate.hpp  Function.hpp
  parse/ SourceSpan.hpp Token.hpp Lexer.hpp ParseError.hpp Parser.hpp Printer.hpp
src/core/ (matching .cpp for non-template classes; parse/ subfolder)
tests/core/ test_signature, test_expression, test_fold, test_substitution,
            test_evaluate, test_lexer, test_parser (round-trip)
```
Each new test needs a `TESTS` entry and an explicit rule in the [Makefile](../../../Makefile) that lists every `src/core/*.cpp` (and the `src/sets/*.cpp` files) it links against.

## Milestones (each ends with green `make test`)
1. **Syntax metadata.** `Sort`, `Operator`, `Signature`, `standard()`. Tests: lookup, arity/sort validation.
2. **Generic terms.** `Expression`, `Constant`, `Variable`, `Apply`, `Builders`, structural `equals`/`hash`. Delete `BinaryOperations`/`UnaryOperations`. Port `test_expression.cpp` to builders; its `toString` expectations must stay identical.
3. **Traversal + semantics.** Visitor, `fold`, `transform`, `Substitution`, `Interpretation<double>`, `Environment`, `evaluate`. Rework `Function`. This closes the "variables can't be evaluated" item in the [root PLAN.md](../../../PLAN.md).
4. **Parser.** `SourceSpan`, `Token`, `Lexer`, `ParseError`, Pratt `Parser`, canonical `Printer`. Tests: precedence/associativity, errors with positions, round-trip property over a corpus of expressions.
5. **Sets through core.** Set operators in the signature, interval literals in the parser, and a `sets::Set` interpretation. Round-trip `([0, 1) U {1, 2})`.
6. **Binders.** `Bind` node, scoped `Environment`, capture-avoiding substitution, set-builder/Σ/∫ syntax.
7. **Second carrier.** A `FiniteField` interpretation and an example demo (`examples/core_demo.cpp`) that evaluates the same parsed term in ℝ and in 𝔽ₚ.

Then write `docs/architecture/core.md` (mermaid class diagram) in the style of `sets.md`.

## Conventions to keep
- Namespace `cppmath::core` (`cppmath::core::parse` for Layer 4); `#pragma once`; includes rooted at `include/`; doc comments per the Documentation rules in [CLAUDE.md](../../../CLAUDE.md#documentation).
- Immutable trees, `unique_ptr` ownership, every node has `clone()`. Operations return new trees.
- Templates (`fold`, `Interpretation`, `Environment`) are header-only.
- Errors: `std::invalid_argument` (bad arity/sort, parse error), `std::out_of_range` (unbound variable, unknown operator in an interpretation).
- Keep it simple: no hash-consing, `shared_ptr` sharing, or expression templates until a benchmark shows they're needed (see "Optimization ideas" in the [root PLAN.md](../../../PLAN.md)).

## Open questions (revisit later)
- Exact numerics: `Integer`/`Rational` literal sorts versus `double` only.
- Whether `Sort` should eventually *be* a `sets::Set` (dependent typing like `x : [0, 1)`) or stay a pure tag.
- Implicit multiplication (`2x`) in the parser.


---

## Status (updated 2026-11-05)

**Milestones 1-4 are implemented** and covered by `test_signature`, `test_expression`, `test_traversal`, `test_evaluate` and `test_parser` (all green under `make test`). Architecture notes: [docs/architecture/core.md](../../../docs/architecture/core.md).

| # | Milestone | State |
|---|---|---|
| 1 | Syntax metadata (`Sort`, `Operator`, `Signature`, `standard()`) | done |
| 2 | Generic terms (`Expression`, `Constant`, `Variable`, `Apply`, `Builders`); old per-operator classes deleted | done |
| 3 | Traversal + semantics (`Visitor`, `fold`, `transform`, `Substitution`, `Interpretation`, `Environment`, `evaluate`, reworked `Function`) | done |
| 4 | Parser (`Lexer`, Pratt `Parser`, `ParseError`, canonical printer, round-trip tests) | done |
| 5 | Sets through core | not started |
| 6 | Binders (`Bind`) | not started |
| 7 | Second carrier demo (`FiniteField`) | partly: `test_evaluate` evaluates one tree over integers mod 7; no `FiniteField` interpretation or demo yet |

### Deviations from the plan above
- **Printer lives in `core/`, not `core/parse/`.** `Expression::toString()` calls it, and core can't depend on the parse layer.
- **Number text is shortest-round-trip formatting**, not a stored source lexeme (`Constant` keeps only the `double`).
- **`Constant` is non-negative and finite.** `-3` is `neg(3)`; this keeps print/parse round-trips exact. `build::num(-3)` does this for you.
- **`Interpretation` and `Substitution` key on operator/variable *name*** (not `const Operator*`), so they work across Signatures that share names.
- **Dropped for now (no operator uses them yet):** `Notation::Postfix`, `Notation::Bracket`, and the "has identity" operator property. `Expression` has no `children()` view; use `Apply::args()`, `fold` or `transform`.
- **`standard()` is arithmetic and elementary functions only** (`+ - * / ^`, prefix `-`, `sin cos tan exp log sqrt abs`, `pi e`). Relations, logic and set operators arrive with milestone 5.
- **`Function` is single-variable** (`Set::contains` takes a `double`), checks the domain *and* codomain on call, and rejects a null rule. The old zero-argument `evaluate()` is gone.
- `core` sources are linked as a whole via a `wildcard` in the Makefile (`CORE_SRC`), so new core `.cpp` files need no Makefile edit.

### Blockers for milestones 5-7: multi-sorted evaluation
Milestone 5 needs expressions that mix sorts (`x in ([0, 1) U {2})` is Real x Set -> Bool). Two gaps in the current design have to be settled first:
1. **One carrier per sort.** `Interpretation<Carrier>` has a single carrier. Mixed-sort evaluation needs `Carrier` to be a variant (e.g. `std::variant<double, bool, std::shared_ptr<const sets::Set>>`) with each operator checking its argument alternatives.
2. **Overloaded operators.** `*` may mean Real x Real, Real x Matrix or Matrix x Matrix, but a `Signature` maps a symbol to exactly one `Operator`. This needs overloads plus a sort-resolution pass after parsing.

Also open: interval literals `[a, b)` versus grouping parentheses, and matrix literals versus intervals.
