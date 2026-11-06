# `core` framework plan

_Last updated: 2026-11-06 · branch `core` · milestones 1-4 of 7 implemented · 52 test cases, 271 assertions, all green under `make test`_

`core` is the **syntax layer** of cppmath: immutable term trees, a vocabulary of operators, a parser and printer for them, and pluggable interpretations that give a tree a value. Architecture diagrams: [docs/architecture/core.md](../../../docs/architecture/core.md). Repo-wide status: [PLAN.md](../../../PLAN.md).

## Status

| # | Milestone | State |
|---|---|---|
| 1 | Syntax metadata: `Sort`, `Operator`, `Signature`, `standard()` | **done** |
| 2 | Generic terms: `Expression`, `Constant`, `Variable`, `Apply`, builders; old per-operator classes deleted | **done** |
| 3 | Traversal + semantics: `Visitor`, `fold`, `transform`, `Substitution`, `Interpretation`, `Environment`, `evaluate`, reworked `Function` | **done** |
| 4 | Parser: `Lexer`, Pratt `Parser`, `ParseError`, canonical printer, round-trip tests | **done** |
| 5 | Sets through core: set operators, interval literals, a `sets::Set` interpretation | not started, blocked on the multi-sorted design (see [Roadmap](#roadmap)) |
| 6 | Binders: `Bind` node, scoped environments, capture-avoiding substitution | not started |
| 7 | Second carrier: a `FiniteField` interpretation and `examples/core_demo.cpp` | partly: `test_evaluate` evaluates one tree over integers mod 7; no `FiniteField` interpretation or demo yet |

| Test target | Cases | Assertions | Covers |
|---|---|---|---|
| `test_signature` | 7 | 39 | sorts, lookups, operator metadata, shape validation, variadics, duplicates, `extend` |
| `test_expression` | 10 | 51 | `Constant`/`Variable` invariants, builders and printing, `Apply` validation, clone/equals/hash, `Function` |
| `test_traversal` | 10 | 25 | `fold`, `transform`, analysis utilities, `Substitution` (simultaneous, sort-checked, `compose`) |
| `test_evaluate` | 8 | 37 | `Environment` scoping, real evaluation, domain errors, a second carrier (Z/7) |
| `test_parser` | 17 | 119 | lexer tokens/positions/longest match, precedence, associativity, prefix minus, calls, variadics, error positions, round-trips |

## Goal and design rule

Everything else (calculus, simplification, set parsing, algebraic evaluation) is a *consumer* of core. Nothing in core knows about `double`, `sin`, or the reals except through a registered `Signature` and `Interpretation`.

**Separate syntax from semantics.**
- *Syntax* (sorts, operators, signatures, terms, parser, printer) knows only shapes.
- *Semantics* (interpretations, environments, evaluation) maps shapes into a carrier type.

## Mathematical model

The class design is meant to keep this table true; a later `cppmath::category` namespace can treat these classes as instances without rewriting core.

| Math | Class | Category-theory reading |
|---|---|---|
| Sort (type of a term: Real, Bool, Set, ...) | `Sort` | object |
| Operation symbol `f : s₁ × … × sₙ → s` | `Operator` | generating (multi)morphism |
| Many-sorted signature Σ | `Signature` | presentation of a (multi)category / Lawvere theory |
| Terms over Σ with variables X | `Expression` (`Constant`, `Variable`, `Apply`; `Bind` planned) | free Σ-algebra T_Σ(X) |
| Σ-algebra with carrier A | `Interpretation<A>` | algebra for the signature functor |
| Variable assignment X → A | `Environment<A>` | arrow out of the generators |
| Evaluation | `fold` / `evaluate` | unique homomorphism T_Σ(X) → A (initiality) |
| Substitution X → T_Σ(Y) | `Substitution`, `compose` | Kleisli arrow / composition for the term monad |

## Architecture as built

All headers are under `include/cppmath/core/`; every public declaration carries a Doxygen comment (see the Documentation rules in [CLAUDE.md](../../../CLAUDE.md#documentation)).

### Layer 0: sorts and operators (syntax metadata)
- **`Sort`**: a named tag compared by name, so other modules can add sorts (`Matrix`, `Group`, ...) without editing core. Built-ins: `Real`, `Bool`, `Set`, `Any`. `Sort::accepts(actual)` treats `Any` as a wildcard on either side. `standard()` only uses `Real` so far.
- **`Operator`**: immutable descriptor with `name` (canonical, unique in a signature, e.g. `"add"`), `symbol` (printed token, e.g. `"+"`), `Arity` (`fixed(n)` or `at_least(n)`), input sorts, output sort, `Notation`, `precedence`, `Assoc`, and `Properties {associative, commutative}`. It has **no evaluation code**: meaning comes from interpretations, derivatives will come from a table in `calculus`.
  - `Notation`: `Atom` (`pi`), `Prefix` (`-x`), `Infix` (`x + y`), `FunctionCall` (`sin(x)`, `max(x, y, z)`).
  - `Assoc`: `Left` or `Right`.
  - The constructor enforces the shape the parser relies on: Atom/Prefix/Infix must have fixed arity 0/1/2; a non-variadic operator lists exactly `arity` input sorts; a variadic one lists at least one and reuses the last for extra arguments.
- **`Signature`**: owns operators at stable heap addresses (non-copyable, movable). `add()` rejects a duplicate name or a duplicate (notation, symbol) pair; `extend(other)` copies another signature's operators in; lookups by name (`find`/`get`) and by symbol (`find_infix`, `find_prefix`, `find_function`, `find_atom`); `operator_symbols()` feeds the lexer (word-like symbols such as `sin` are excluded because they lex as identifiers).
  - **`Signature::standard()`** (function-local static): infix `+ - * / ^`, prefix `-`, functions `sin cos tan exp log sqrt abs`, atoms `pi e`.
  - **Lifetime rule:** an `Apply` holds a non-owning `const Operator*`, so the `Signature` must outlive every expression built from it. `standard()` always does.

### Layer 1: terms (the AST)
- **`Expression`** (abstract, immutable, owned as `ExprPtr = std::unique_ptr<Expression>`): `sort()`, `clone()`, `equals()` (structural), `hash()`, `accept(visitor)`, and a non-virtual `toString()` that delegates to the printer. There is no `children()` view; use `Apply::args()`, `fold` or `transform`.
- **`Constant`**: a numeric literal, a finite non-negative `double`.
- **`Variable`**: an identifier name plus a `Sort` (default `Real`).
- **`Apply`**: `const Operator*` plus `std::vector<ExprPtr>`. The constructor throws `std::invalid_argument` on a wrong argument count, a null argument, or a sort mismatch, so every `Apply` that exists is well-sorted. Atoms such as `pi` are an `Apply` with no arguments (there is no separate `NamedConstant` class).
- **Builders** (`Builders.hpp`, namespace `cppmath::core::build`): `num`, `var`, `pi`, `euler`, `add sub mul div pow neg`, `sin cos tan exp log sqrt abs`. They replace the old `AddExpression`/`SinExpression`/... classes, which are deleted. `num(-3)` returns `neg(3)`; `var` rejects the names of standard atoms.
- **Not built yet:** `Bind` (milestone 6).

### Layer 2: traversal
- **`ExpressionVisitor`**: one `visit` per node kind (`Constant`, `Variable`, `Apply`). Operators are data, so it does not grow with the operator set.
- **`fold<R>(expr, on_constant, on_variable, on_apply)`**: the catamorphism; `on_apply(const Operator&, std::vector<R>)` receives the folded children. Printing, evaluation, `free_variables`, `node_count` and `tree_depth` are folds.
- **`transform(expr, rewrite)`**: bottom-up rewrite; `rewrite` sees each node after its children were rewritten and its output is not traversed again.
- **`Substitution`**: name → expression map with `bind`, `binds`, `size`, `apply`. Application is simultaneous (`{x→y, y→x}` swaps) and sort-checked (`std::invalid_argument`). `compose(first, second)` satisfies `apply(compose(s, t), e) == t.apply(s.apply(e))`.
- **Analysis** (`Analysis.hpp`): `free_variables`, `contains_variable`, `node_count`, `tree_depth`.

### Layer 3: semantics
- **`Interpretation<Carrier>`** (header-only template): constructed with a literal embedding `double → Carrier`; `define(op_name, fn)` gives each operator a meaning; `apply` throws `std::out_of_range` for an operator with no meaning. Keyed by operator *name*, so it works across signatures that share names.
- **`Environment<Carrier>`**: `bind`, `has`, `lookup` (`std::out_of_range` if unbound), with an optional non-owning parent scope that shadows. The parent scope exists for the binders of milestone 6.
- **`evaluate(expr, interp, env)`**: the fold into `Carrier`. **`evaluate_real(expr, env)`** uses **`real_interpretation()`** (`double`).
- **`Function`**: `name`, `param` (default `"x"`), non-owning nullable `domain`/`codomain` (`const sets::Set*`), owned non-null `rule`. `operator()(double)` throws `std::invalid_argument` if the argument is outside the domain or the result is outside the codomain.

### Layer 4: concrete syntax (`core/parse/`, plus the printer)
- **`SourceSpan`** (offset, length, 1-based line and column) and **`Token`/`TokenKind`** (`Number`, `Identifier`, `Operator`, `LParen`, `RParen`, `Comma`, `End`).
- **`Lexer`**: longest-match on the signature's operator symbols, so new operators need no lexer change; UTF-8 symbols match bytewise; identifiers are ASCII `[A-Za-z_][A-Za-z0-9_]*`; numbers are `digits[.digits][e[+-]digits]` (an `e` with no digits after it is left for the next token, so `2e` is the number 2 then the identifier `e`).
- **`ParseError`**: derives from `std::invalid_argument`; `what()` is `line:column: message` and `span()` locates the problem.
- **`Parser`**: Pratt/precedence climbing with binding powers from `Operator::precedence()`/`assoc()`. An identifier is, in order: a function call (if followed by `(`), an atom of the signature (`pi`, `e`), or a variable. A bare function name without `(` is an error. Arity and sort complaints from `Apply` are re-thrown as positioned `ParseError`s.
- **Printer** (`Printer.hpp`, in `core/`, not `core/parse/`): `print(expr)` is a fold driven by `Notation`; `Expression::toString()` calls it. Number text comes from `format_number`, which delegates to `utils::format_number` in [`cppmath/utils/format.hpp`](../utils/format.hpp) (shortest decimal that parses back exactly; `sets` shares it).

## Behaviour contracts

**Canonical text.** Fully parenthesised, so reading it back needs no precedence knowledge:

| Notation | Printed as |
|---|---|
| Atom | `pi` |
| Prefix | `(-x)` |
| Infix | `(x + y)` |
| FunctionCall | `sin(x)`, `max(1, x, (3 + 4))` |

**Round trip (tested over a corpus):** `parse(e.toString())` equals `e`, and `print(parse(s))` is a fixed point of `parse` then `print`. This is why `Constant` is never negative (the text `-3` parses as `neg(3)`) and why `Variable` names equal to an atom (`pi`, `e`) are rejected by `build::var`: they would parse back as something else.

**Precedence** (loosest to tightest): `+ -` (10), `* /` (20), prefix `-` (30), `^` (40, right-associative). So `-x^2` is `-(x^2)`, `-x*y` is `(-x)*y`, `2^3^2` is `2^(3^2)`, and `1-2-3` is `(1-2)-3`.

**Errors.**

| Condition | Exception |
|---|---|
| Bad arity/sort/null argument to `Apply`; invalid `Constant` or `Variable`; duplicate operator in a `Signature`; sort-incorrect substitution; domain/codomain violation in `Function` | `std::invalid_argument` |
| Bad domain while evaluating over the reals: `1/0`, `log(x<=0)`, `sqrt(x<0)`, `pow` with an undefined result | `std::invalid_argument` |
| Malformed text | `ParseError` (an `invalid_argument` with a `SourceSpan`) |
| Unbound variable; operator not defined in an interpretation; unknown name in `Signature::get` | `std::out_of_range` |

**Known limits.** No implicit multiplication (`2x` is a `ParseError`); identifiers are ASCII; `Function` is single-variable; `standard()` has no relations, logic or set operators yet; numeric literals are `double` only.

## Layout and build

```
include/cppmath/core/
  Sort.hpp  Operator.hpp  Signature.hpp
  Expression.hpp  Constant.hpp  Variable.hpp  Apply.hpp  Builders.hpp
  Visitor.hpp  Fold.hpp  Analysis.hpp  Substitution.hpp
  Interpretation.hpp  Environment.hpp  Evaluate.hpp  RealInterpretation.hpp  Function.hpp
  Printer.hpp
  parse/  SourceSpan.hpp  Token.hpp  Lexer.hpp  ParseError.hpp  Parser.hpp
src/core/    Operator, Signature, Constant, Variable, Apply, Builders, Analysis,
             Substitution, RealInterpretation, Function, Printer .cpp;  parse/Lexer.cpp, parse/Parser.cpp
tests/core/  test_signature  test_expression  test_traversal  test_evaluate  test_parser
```

Templates (`fold`, `transform`, `Interpretation`, `Environment`, `evaluate`) are header-only. The Makefile links core as a unit through `CORE_SRC` (a `wildcard` over `src/core/*.cpp` and `src/core/parse/*.cpp`), so a new core `.cpp` needs no Makefile edit; a new test target still needs a `TESTS` entry and a rule, and links `$(SETS_SRC)` as well because `Function` takes `sets::Set`.

## Roadmap

### Milestones 5-7 and what blocks them
Milestone 5 needs expressions that mix sorts (`x in ([0, 1) U {2})` is Real × Set → Bool). Two gaps in the current design have to be settled first:

1. **One carrier per sort.** `Interpretation<Carrier>` has a single carrier. Mixed-sort evaluation needs `Carrier` to be a variant (for example `std::variant<double, bool, std::shared_ptr<const sets::Set>>`) with each operator checking its argument alternatives. Sets and matrices should be carried as `shared_ptr<const T>` (they are immutable and costly to copy).
2. **Overloaded operators.** `*` may mean Real × Real, Real × Matrix or Matrix × Matrix, but a `Signature` maps a symbol to exactly one `Operator`. It needs several operators per symbol, plus a sort-resolution pass after parsing that picks the overload once variable sorts are known.

Also to settle for milestone 5: interval literals `[a, b)` versus grouping parentheses (proposed rule: `(` followed by `expr , expr` and then `)` or `]` is an interval literal), and matrix literals versus intervals. Dependency direction: core depends only on `sets`; other modules (`linalg`, `abstract_algebra`) supply their own interpretations (for example `linalg/interpretation.hpp`) and extend `Signature::standard()` with their sorts and operators.

| Milestone | Work |
|---|---|
| 5 | Add relations/logic/set operators and the `Bool`/`Set` sorts to a signature; interval and set literals in the lexer/parser; overload resolution; a `sets::Set` interpretation so a parsed `([0, 1) U {1, 2})` evaluates to a real `UnionSet`. Round-trip `([0, 1) U {1, 2})`. |
| 6 | `Bind` node (binder operator, bound `Variable`s, body, optional bounds) covering λ, ∀/∃, Σ/∏, ∫ and set-builder `{x ∈ A | P}`; scoped `Environment` (parent scope already exists); capture-avoiding `Substitution`. |
| 7 | A `FiniteField` interpretation (the `abstract_algebra` examples already expose `one()` and per-operation identities) and `examples/core_demo.cpp` evaluating one parsed term in ℝ and in 𝔽ₚ. |

### Enabled by core, not part of it
- `calculus::differentiate`: a per-operator derivative rule table plus `transform` (fills `calculus/diff.hpp`).
- `core::simplify`: rewrite rules (constant folding, `x+0`, `x*1`, `x^1`) driven by `Properties`.
- `cppmath::category`: `Category`/`Functor`/natural transformation concepts. `Signature` ⇒ theory, `Interpretation` ⇒ model, `fold` ⇒ the induced homomorphism.

## Where the implementation departs from the first design

| First design | As built | Why |
|---|---|---|
| Printer in `core/parse/` | `core/Printer.hpp` | `Expression::toString()` calls it, and core cannot depend on the parse layer |
| `Constant` keeps the source lexeme | `Constant` keeps a `double`; `format_number` prints the shortest round-trip text | simpler, and the round trip still holds |
| `Constant` may be any number | finite and non-negative only | `-3` must parse back to the same tree it prints from |
| `Interpretation`/`Substitution` keyed by `const Operator*` / `Variable` | keyed by *name* | works across signatures that share names |
| `Notation::Postfix`, `Notation::Bracket`; `Assoc::None` | omitted | no operator uses them yet |
| Operator `Properties` includes "has identity" | only `associative`, `commutative` | an identity element is a value in a carrier, not syntax. `abstract_algebra::AlgebraicOperation` now has `has_identity()`/`identity(structure)`; the matching place in core is an `Interpretation`, if it is ever needed |
| `Expression::children()` view | none | `Apply::args()`, `fold` and `transform` cover every use |
| `NamedConstant` class (`π`, `e`, `∅`, `ℝ`) | an `Apply` of a nullary `Atom` operator, built by `build::pi()`/`euler()` | one node kind |
| `standard()` includes relations, logic, set operators | arithmetic and elementary functions only | those need the multi-sorted design above |
| `Function` with several parameters | one parameter | `Set::contains` takes a single `double` |
| `Sort::Integer` | omitted | literals are `double` only for now |

## Conventions

- Namespace `cppmath::core` (`cppmath::core::parse` for the lexer/parser, `cppmath::core::build` for builders); `#pragma once`; includes rooted at `include/`.
- Immutable trees, `unique_ptr` ownership, every node has `clone()`. Operations return new trees.
- Doc comments on every public declaration, per [CLAUDE.md](../../../CLAUDE.md#documentation); check with `clang++ -std=c++17 -Iinclude -Wdocumentation -fsyntax-only`.
- Keep it simple: no hash-consing, `shared_ptr` sharing or expression templates until a benchmark shows they are needed (see "Optimization ideas" in the [root PLAN.md](../../../PLAN.md)).

## Open questions

- Exact numerics: `Integer`/`Rational` literal sorts versus `double` only.
- Whether `Sort` should eventually *be* a `sets::Set` (dependent typing like `x : [0, 1)`) or stay a pure tag.
- Implicit multiplication (`2x`) in the parser.
- Where the variant `Value` type for multi-sorted evaluation lives (core, or each consuming module).
