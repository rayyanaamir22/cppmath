# `core` framework plan

_Last updated: 2026-11-17 · branch `core` · milestones 1-4, 6 and 7 of 7 implemented (5 is blocked on a design decision) · `core` has 95 test cases / 673 assertions, plus 20 more cases in its first two consumers; all green under `make test`_

`core` is the **syntax layer** of cppmath: immutable term trees, a vocabulary of operators, a parser and printer for them, and pluggable interpretations that give a tree a value. Architecture diagrams: [docs/architecture/core.md](../../../docs/architecture/core.md). Repo-wide status: [PLAN.md](../../../PLAN.md).

## Status

| # | Milestone | State |
|---|---|---|
| 1 | Syntax metadata: `Sort`, `Operator`, `Signature`, `standard()` | **done** |
| 2 | Generic terms: `Expression`, `Constant`, `Variable`, `Apply`, builders; old per-operator classes deleted | **done** |
| 3 | Traversal + semantics: `Visitor`, `fold`, `transform`, `Substitution`, `Interpretation`, `Environment`, `evaluate`, reworked `Function` | **done** |
| 4 | Parser: `Lexer`, Pratt `Parser`, `ParseError`, canonical printer, round-trip tests | **done** |
| 5 | Sets through core: set operators, interval literals, a `sets::Set` interpretation | **not started**: blocked on the multi-sorted design (see [Roadmap](#roadmap)) |
| 6 | Binders: `Bind` node, scoped evaluation, capture-avoiding substitution, `sum`/`prod` | **done** (λ, ∀/∃, ∫ and set-builder are not built; they need milestone 5's sorts) |
| 7 | Second carrier: a `FiniteField` interpretation and `examples/core_demo.cpp` | **done** |
| + | `core::simplify` and `calculus::differentiate` (the "enabled by core" consumers) | **done** |

| Test target | Cases | Assertions | Covers |
|---|---|---|---|
| `test_signature` | 10 | 65 | sorts, lookups, operator metadata, shape validation (incl. binders), variadics, duplicates, `extend`, reserved words |
| `test_expression` | 10 | 51 | `Constant`/`Variable` invariants, builders and printing, `Apply` validation, clone/equals/hash, `Function` |
| `test_traversal` | 10 | 25 | `fold`, `transform`, analysis utilities, `Substitution` (simultaneous, sort-checked, `compose`) |
| `test_evaluate` | 8 | 37 | `Environment` scoping, real evaluation, domain errors, a second carrier (Z/7) |
| `test_binders` | 29 | 149 | `Bind` validation/equality, free variables, α-equivalence, `fold`/`transform` with binders, shadowing and capture-avoiding substitution (incl. the substitution lemma), `sum`/`prod` evaluation, parsing, round-trips |
| `test_simplify` | 11 | 227 | identities, sign handling, exact constant folding, idempotence, value preservation over a corpus |
| `test_parser` | 17 | 119 | lexer tokens/positions/longest match, precedence, associativity, prefix minus, calls, variadics, error positions, round-trips |
| *consumers:* `test_field_interpretation` | 9 | 150 | one parsed tree in ℝ and in F_p (ring-homomorphism check for several primes) |
| *consumers:* `test_diff` | 11 | 89 | derivative rules, chain rule, sums, custom rules, agreement with numerical differentiation on 23 functions |

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
| Terms over Σ with variables X | `Expression` (`Constant`, `Variable`, `Apply`, `Bind`) | free Σ-algebra T_Σ(X) (with binding) |
| Σ-algebra with carrier A | `Interpretation<A>` | algebra for the signature functor |
| Variable assignment X → A | `Environment<A>` | arrow out of the generators |
| Evaluation | `evaluate` (a fold, with lazy binder bodies) | unique homomorphism T_Σ(X) → A (initiality) |
| Substitution X → T_Σ(Y) | `Substitution`, `compose` | Kleisli arrow / composition for the term monad |

## Architecture as built

Core headers are under `include/cppmath/core/`; every public declaration carries a Doxygen comment (see the Documentation rules in [CLAUDE.md](../../../CLAUDE.md#documentation)).

### Layer 0: sorts and operators (syntax metadata)
- **`Sort`**: a named tag compared by name, so other modules can add sorts (`Matrix`, `Group`, ...) without editing core. Built-ins: `Real`, `Bool`, `Set`, `Any`. `Sort::accepts(actual)` treats `Any` as a wildcard on either side. `standard()` only uses `Real` so far.
- **`Operator`**: immutable descriptor with `name` (canonical, unique in a signature, e.g. `"add"`), `symbol` (printed token, e.g. `"+"`), `Arity` (`fixed(n)` or `at_least(n)`), input sorts, output sort, `Notation`, `precedence`, `Assoc`, `Properties {associative, commutative}`, and for binders a `bound_sort`. It has **no evaluation code**: meaning comes from interpretations, derivatives from a rule table in `calculus`.
  - `Notation`: `Atom` (`pi`), `Prefix` (`-x`), `Infix` (`x + y`), `FunctionCall` (`sin(x)`, `max(x, y, z)`), `Binder` (`sum(i, 1, 10, body)`).
  - `Assoc`: `Left` or `Right`.
  - The constructor enforces the shape the parser relies on: Atom/Prefix/Infix must have fixed arity 0/1/2; a binder has fixed arity ≥ 1 (its operands, the last being the body; the bound variable is *not* an argument); a non-variadic operator lists exactly `arity` input sorts; a variadic one lists at least one and reuses the last for extra arguments.
- **`Signature`**: owns operators at stable heap addresses (non-copyable, movable). `add()` rejects a duplicate name or a duplicate (notation, symbol) pair; `extend(other)` copies another signature's operators in; lookups by name (`find`/`get`) and by symbol (`find_infix`, `find_prefix`, `find_function`, `find_atom`, `find_binder`); `is_reserved_word(w)` is true for atoms, functions and binders; `operator_symbols()` feeds the lexer (word-like symbols are excluded because they lex as identifiers).
  - **`Signature::standard()`** (function-local static): infix `+ - * / ^`, prefix `-`, functions `sin cos tan exp log sqrt abs`, atoms `pi e`, binders `sum prod`.
  - **Lifetime rule:** an `Apply`/`Bind` holds a non-owning `const Operator*`, so the `Signature` must outlive every expression built from it. `standard()` always does.

### Layer 1: terms (the AST)
- **`Expression`** (abstract, immutable, owned as `ExprPtr = std::unique_ptr<Expression>`): `sort()`, `clone()`, `equals()` (structural), `hash()`, `accept(visitor)`, and a non-virtual `toString()` that delegates to the printer. There is no `children()` view; use `Apply::args()`, `Bind::operands()`, `fold` or `transform`.
- **`Constant`**: a numeric literal, a finite non-negative `double`.
- **`Variable`**: an identifier name plus a `Sort` (default `Real`).
- **`Apply`**: `const Operator*` plus `std::vector<ExprPtr>`. The constructor throws `std::invalid_argument` on a wrong argument count, a null argument, or a sort mismatch, so every `Apply` that exists is well-sorted. Atoms such as `pi` are an `Apply` with no arguments.
- **`Bind`**: a binder `Operator*`, a bound `Variable`, and operands (bounds first, **body last**). Same well-sortedness guarantee as `Apply`. The variable is in scope **only in the body**: the bounds see the enclosing scope, so `sum(i, 1, i, i)` has a free `i` (the upper bound). `equals()` is syntactic; use `alpha_equivalent` to compare up to renaming.
- **Builders** (`Builders.hpp`, namespace `cppmath::core::build`): `num`, `var`, `pi`, `euler`, `add sub mul div pow neg`, `sin cos tan exp log sqrt abs`, `sum`, `prod`. `num(-3)` returns `neg(3)`; `var` and the binder builders reject reserved words.

### Layer 2: traversal
- **`ExpressionVisitor`**: one `visit` per node kind (`Constant`, `Variable`, `Apply`, `Bind`). Operators are data, so it does not grow with the operator set.
- **`fold<R>(expr, on_constant, on_variable, on_apply, on_bind)`**: the catamorphism; `on_apply(const Operator&, std::vector<R>)` and `on_bind(const Bind&, std::vector<R>)` receive the folded children (for a `Bind`: the bounds, then the body). A 3-handler overload (no `on_bind`) exists for binder-free trees and **throws `std::invalid_argument` if it meets a binder**, so a binder is never silently mishandled. Printing, `free_variables`, `node_count` and `tree_depth` are folds.
- **`transform(expr, rewrite)`**: bottom-up rewrite; `rewrite` sees each node after its children were rewritten and its output is not traversed again. It is **not scope-aware** (it sees a bound variable like any other).
- **`Substitution`**: name → expression map with `bind`, `binds`, `size`, `apply`. Application is simultaneous (`{x→y, y→x}` swaps), sort-checked, and **scope-aware**: it never replaces a shadowed variable, and renames a binder's variable (`i` → `i_1`, `i_2`, ... avoiding every name in sight) *only when* a replacement would otherwise be captured. `compose(first, second)` satisfies `apply(compose(s, t), e) == t.apply(s.apply(e))` exactly without binders, and up to α-equivalence with them.
- **Analysis** (`Analysis.hpp`): `free_variables` (binder-aware), `contains_variable`, `node_count`, `tree_depth`, `alpha_equivalent`.
- **`simplify`** (`Simplify.hpp`): conservative algebraic clean-up. Identities (`x+0`, `x*1`, `x*0`, `x^1`, `x^0`, `x-x`, `--x`, ...), sign normalisation (negations pulled outward and cancelled), and **exact** constant folding (`+ - *`, exact quotients, small non-negative integer powers, and the exact values `sin(0)`, `cos(0)`, `exp(0)`, `log(1)`, ...). Inexact results (`1/3`, `sin(1)`) stay symbolic. Idempotent; never mutates its input. Trade-off, documented: like any CAS simplifier it treats its identities as always valid, so it can drop a sub-expression that would have failed to evaluate (`0 * log(0)` → `0`).

### Layer 3: semantics
- **`Interpretation<Carrier>`** (header-only template): constructed with a literal embedding `double → Carrier`; `define(op_name, fn)` gives each operator a meaning; `define_binder(op_name, fn)` gives a binder one: `fn(bounds, body)` gets the already-evaluated bounds and a callback that evaluates the body for one value of the bound variable. `apply` / `apply_binder` throw `std::out_of_range` for an operator with no meaning. Keyed by operator *name*, so it works across signatures that share names.
- **`Environment<Carrier>`**: `bind`, `has`, `lookup` (`std::out_of_range` if unbound), with an optional non-owning parent scope that shadows. Binder evaluation uses it: each body evaluation runs in a child scope that binds the variable.
- **`evaluate(expr, interp, env)`**: a visitor, not a `fold`, because a binder's body must be evaluated lazily and repeatedly. **`evaluate_real(expr, env)`** uses **`real_interpretation()`** (`double`), which defines `sum`/`prod` over integer bounds (finite, at most 10,000,000 terms, else `std::invalid_argument`; an empty range gives 0 / 1).
- **`Function`**: `name`, `param` (default `"x"`), non-owning nullable `domain`/`codomain` (`const sets::Set*`), owned non-null `rule`. `operator()(double)` throws `std::invalid_argument` if the argument is outside the domain or the result is outside the codomain.

### Layer 4: concrete syntax (`core/parse/`, plus the printer)
- **`SourceSpan`** (offset, length, 1-based line and column) and **`Token`/`TokenKind`** (`Number`, `Identifier`, `Operator`, `LParen`, `RParen`, `Comma`, `End`).
- **`Lexer`**: longest-match on the signature's operator symbols, so new operators need no lexer change; UTF-8 symbols match bytewise; identifiers are ASCII `[A-Za-z_][A-Za-z0-9_]*`; numbers are `digits[.digits][e[+-]digits]` (an `e` with no digits after it is left for the next token, so `2e` is the number 2 then the identifier `e`).
- **`ParseError`**: derives from `std::invalid_argument`; `what()` is `line:column: message` and `span()` locates the problem.
- **`Parser`**: Pratt/precedence climbing with binding powers from `Operator::precedence()`/`assoc()`. An identifier is, in order: a binder or function call (if followed by `(`), an atom of the signature (`pi`, `e`), or a variable. A bare function or binder name without `(` is an error. In a binder call the first argument must be an identifier (the bound variable, never a reserved word), followed by the operands. Arity and sort complaints are re-thrown as positioned `ParseError`s.
- **Printer** (`Printer.hpp`, in `core/`, not `core/parse/`): `print(expr)` is a fold driven by `Notation`; `Expression::toString()` calls it. Number text comes from `format_number`, which delegates to `utils::format_number` in [`cppmath/utils/format.hpp`](../utils/format.hpp) (shortest decimal that parses back exactly; `sets` shares it).

### Consumers outside `core`
- **`abstract_algebra::example::finite_field_interpretation(field)`** (`abstract_algebra/field/FieldInterpretation.hpp`): meaning of `add sub mul div neg` in Z/pZ, numerals reduced mod p (they must be integers). Everything else is undefined and throws `std::out_of_range`. **`pow` is undefined on purpose**: its exponent would be a field element, so `x^7` in F_7 would read the exponent as 0 (giving 1) where Fermat gives x. Evaluating `+ - *` terms in F_p agrees with evaluating over ℝ and reducing mod p, which the tests check for several primes. Dependency direction: `abstract_algebra` depends on `core`, never the reverse.
- **`calculus::differentiate(expr, var, rules)`** (`calculus/diff.hpp`): a `DerivativeRules` table keyed by operator name (`standard()` covers every standard operator; copy it and `define` a rule for a new operator), chain rule applied by the table, result passed through `simplify`. Handles `sum` (differentiating under the binder) when the bounds don't depend on `var`; `prod`, an operator with no rule, or `var`-dependent bounds throw `std::invalid_argument`.
- **`examples/core_demo.cpp`**: parses one expression and evaluates it in ℝ and F_7, evaluates a sum, shows capture-avoiding substitution, differentiation and simplification, and a positioned parse error.

## Behaviour contracts

**Canonical text.** Fully parenthesised, so reading it back needs no precedence knowledge:

| Notation | Printed as |
|---|---|
| Atom | `pi` |
| Prefix | `(-x)` |
| Infix | `(x + y)` |
| FunctionCall | `sin(x)`, `max(1, x, (3 + 4))` |
| Binder | `sum(i, 1, 10, (i ^ 2))` (bound variable first, body last) |

**Round trip (tested over a corpus, with and without binders):** `parse(e.toString())` equals `e`, and `print(parse(s))` is a fixed point of `parse` then `print`. This is why `Constant` is never negative (the text `-3` parses as `neg(3)`) and why reserved words (`pi`, `e`, `sin`, `sum`, ...) are rejected as variable names by `build::var`: they would parse back as something else.

**Precedence** (loosest to tightest): `+ -` (10), `* /` (20), prefix `-` (30), `^` (40, right-associative). So `-x^2` is `-(x^2)`, `-x*y` is `(-x)*y`, `2^3^2` is `2^(3^2)`, and `1-2-3` is `(1-2)-3`.

**Errors.**

| Condition | Exception |
|---|---|
| Bad arity/sort/null argument to `Apply` or `Bind`; invalid `Constant` or `Variable`; duplicate operator in a `Signature`; sort-incorrect substitution; domain/codomain violation in `Function`; `fold` (3-handler) meeting a binder; `differentiate` with no rule | `std::invalid_argument` |
| Bad domain while evaluating over the reals: `1/0`, `log(x<=0)`, `sqrt(x<0)`, `pow` with an undefined result; non-integer or too many `sum`/`prod` terms | `std::invalid_argument` |
| Malformed text | `ParseError` (an `invalid_argument` with a `SourceSpan`) |
| Unbound variable; operator or binder not defined in an interpretation; unknown name in `Signature::get` | `std::out_of_range` |

**Known limits.** No implicit multiplication (`2x` is a `ParseError`); identifiers are ASCII; `Function` is single-variable; `standard()` has no relations, logic or set operators yet; numeric literals are `double` only; `Bind::equals` is syntactic (use `alpha_equivalent`); `transform` is not scope-aware; `simplify` does not combine like terms (`x + x` stays) or expand products; there is no integral or partial-derivative support.

## Layout and build

```
include/cppmath/core/
  Sort.hpp  Operator.hpp  Signature.hpp
  Expression.hpp  Constant.hpp  Variable.hpp  Apply.hpp  Bind.hpp  Builders.hpp
  Visitor.hpp  Fold.hpp  Analysis.hpp  Substitution.hpp  Simplify.hpp
  Interpretation.hpp  Environment.hpp  Evaluate.hpp  RealInterpretation.hpp  Function.hpp
  Printer.hpp
  parse/  SourceSpan.hpp  Token.hpp  Lexer.hpp  ParseError.hpp  Parser.hpp
src/core/    Operator, Signature, Constant, Variable, Apply, Bind, Builders, Analysis, Substitution,
             Simplify, RealInterpretation, Function, Printer .cpp;  parse/Lexer.cpp, parse/Parser.cpp
tests/core/  test_signature  test_expression  test_traversal  test_evaluate  test_binders
             test_simplify  test_parser
consumers:   include/cppmath/abstract_algebra/field/FieldInterpretation.hpp  (+ src/, tests/)
             include/cppmath/calculus/diff.hpp                                (+ src/calculus/diff.cpp, tests/calculus/test_diff.cpp)
             examples/core_demo.cpp
```

Templates (`fold`, `transform`, `Interpretation`, `Environment`, `evaluate`) are header-only. The Makefile links core as a unit through `CORE_SRC` (a `wildcard` over `src/core/*.cpp` and `src/core/parse/*.cpp`), so a new core `.cpp` needs no Makefile edit; a new test target still needs a `TESTS` entry and a rule, and links `$(SETS_SRC)` as well because `Function` takes `sets::Set`.

## Roadmap

### Milestone 5 and what blocks it
Milestone 5 needs expressions that mix sorts (`x in ([0, 1) U {2})` is Real × Set → Bool). Two gaps in the current design have to be settled first:

1. **One carrier per sort.** `Interpretation<Carrier>` has a single carrier. Mixed-sort evaluation needs `Carrier` to be a variant (for example `std::variant<double, bool, std::shared_ptr<const sets::Set>>`) with each operator checking its argument alternatives. Sets and matrices should be carried as `shared_ptr<const T>` (they are immutable and costly to copy).
2. **Overloaded operators.** `*` may mean Real × Real, Real × Matrix or Matrix × Matrix, but a `Signature` maps a symbol to exactly one `Operator`. It needs several operators per symbol, plus a sort-resolution pass after parsing that picks the overload once variable sorts are known.

Also to settle: interval literals `[a, b)` versus grouping parentheses (proposed rule: `(` followed by `expr , expr` and then `)` or `]` is an interval literal), and matrix literals versus intervals. Dependency direction: core depends only on `sets`; other modules (`linalg`, `abstract_algebra`) supply their own interpretations, as `FieldInterpretation` does, and extend `Signature::standard()` with their sorts and operators.

| Milestone | Work |
|---|---|
| 5 | Add relations/logic/set operators and the `Bool`/`Set` sorts to a signature; interval and set literals in the lexer/parser; overload resolution; a `sets::Set` interpretation so a parsed `([0, 1) U {1, 2})` evaluates to a real `UnionSet`. Round-trip `([0, 1) U {1, 2})`. |
| 6 (rest) | More binders once their sorts exist: λ, ∀/∃, ∫, set-builder `{x ∈ A \| P}`. They reuse `Bind`, `define_binder` and capture-avoiding substitution unchanged. |

### Natural next consumers
- `calculus`: partial derivatives and gradients over several variables, `prod` (needs the logarithmic derivative and a non-zero guard), integration (rule table plus a numerical fallback via a binder).
- `core::simplify`: combine like terms, `x*x → x^2`, `x/x`, rational-function normal forms, using `Properties`.
- `cppmath::category`: `Category`/`Functor`/natural transformation concepts. `Signature` ⇒ theory, `Interpretation` ⇒ model, `fold` ⇒ the induced homomorphism.

## Where the implementation departs from the first design

| First design | As built | Why |
|---|---|---|
| Printer in `core/parse/` | `core/Printer.hpp` | `Expression::toString()` calls it, and core cannot depend on the parse layer |
| `Constant` keeps the source lexeme | `Constant` keeps a `double`; `format_number` prints the shortest round-trip text | simpler, and the round trip still holds |
| `Constant` may be any number | finite and non-negative only | `-3` must parse back to the same tree it prints from |
| `Interpretation`/`Substitution` keyed by `const Operator*` / `Variable` | keyed by *name* | works across signatures that share names |
| `Notation::Postfix`, `Notation::Bracket`; `Assoc::None` | omitted | no operator uses them yet |
| Operator `Properties` includes "has identity" | only `associative`, `commutative` | an identity element is a value in a carrier, not syntax. `abstract_algebra::AlgebraicOperation` has `has_identity()`/`identity(structure)`; the matching place in core would be an `Interpretation` |
| `Expression::children()` view | none | `Apply::args()`, `Bind::operands()`, `fold` and `transform` cover every use |
| `NamedConstant` class (`π`, `e`, `∅`, `ℝ`) | an `Apply` of a nullary `Atom` operator, built by `build::pi()`/`euler()` | one node kind |
| `standard()` includes relations, logic, set operators | arithmetic, elementary functions, `sum`/`prod` | relations/logic/sets need the multi-sorted design above |
| `Function` with several parameters | one parameter | `Set::contains` takes a single `double` |
| `Sort::Integer` | omitted | literals are `double` only for now |
| Binders as one more `Apply` | a separate `Bind` node, `Notation::Binder`, and `Interpretation::define_binder` | a binder's body must be evaluated lazily and repeatedly under a changing binding, and substitution/analysis need to know which operand is in scope |
| `evaluate` is a `fold` | a visitor | same reason: the body is not a pre-computed child result |
| `fold` with three handlers | four (adds `on_bind`); the 3-handler form remains for binder-free trees and throws if it meets a binder | scope-aware folds need the node (for its bound variable's name) |
| `Substitution::apply` built on `transform` | its own scope-aware recursion | `transform` visits children before parents, so it cannot skip a shadowed subtree or rename a binder |
| `build::var` rejected only atom names | rejects every reserved word (atoms, functions, binders) | found while adding binders: `var("sin")` printed text that the parser rejected |
| `compose` property `t(s(e))` exact | exact without binders, up to α-equivalence with them | capture avoidance may choose a different fresh name along each route |

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
- Whether `Bind::equals` should become α-equivalence (it is syntactic now, and `hash` would have to follow).
