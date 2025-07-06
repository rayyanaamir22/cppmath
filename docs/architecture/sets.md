# Set Architecture in cppmath

This document describes the design and relationships of the set classes in the cppmath library, which provide a flexible, extensible, and symbolic framework for mathematical set operations.

## Overview

The set hierarchy is designed to support both concrete sets (like intervals and finite sets) and composite sets (like unions and intersections), with a focus on:
- **Immutability:** All set operations return new set objects, never modifying the originals.
- **Polymorphism:** All set types inherit from a common abstract base and are manipulated via virtual methods.
- **Smart Pointers:** All sets are managed with `std::unique_ptr` for safe, automatic memory management.
- **AST-Ready String Representations:** Every set can be serialized to a string suitable for parsing by an abstract syntax tree (AST) parser.

## Class Hierarchy

```
Set (abstract)
├── IntervalSet
├── FiniteSet
├── UnionSet
├── IntersectionSet
├── EmptySet (singleton)
└── UniversalSet (singleton)
```

### 1. `Set` (Abstract Base)
Defines the interface for all sets:
- `contains(double value)`: Membership test.
- `set_union(const Set&)`: Returns the union with another set.
- `intersect(const Set&)`: Returns the intersection with another set.
- `complement()`: Returns the complement within the universal set.
- `toString()`: Returns a string representation (AST-friendly).
- `clone()`: Deep copy.
- `equals(const Set&)`: Mathematical equality.

### 2. `IntervalSet`
Represents a real interval, e.g., `[0, 1)`. Supports open/closed bounds. Implements all `Set` methods. String representation: `[a, b)`, `(a, b]`, etc.

### 3. `FiniteSet`
Represents a finite set of real numbers, e.g., `{1, 2, 3}`. Implements all `Set` methods. String representation: `{1, 2, 3}`.

### 4. `UnionSet` and `IntersectionSet`
Composite sets representing unions and intersections of two sets. These can be nested to form complex set expressions. String representations: `(A U B)`, `(A ∩ B)`.

### 5. `EmptySet` and `UniversalSet`
Singletons representing the empty set (`∅`) and the universal set (`ℝ`).

## Method Interactions
- All set operations (`set_union`, `intersect`, `complement`) return new `std::unique_ptr<Set>` objects.
- Composite sets (unions/intersections) store their operands as `std::unique_ptr<Set>`.
- Methods use `dynamic_cast` to optimize operations when possible (e.g., merging intervals).
- String representations are canonical and suitable for AST parsing.

## Example
```cpp
std::unique_ptr<Set> s1 = std::make_unique<IntervalSet>(0, 1, true, false); // [0, 1)
std::unique_ptr<Set> s2 = std::make_unique<FiniteSet>({1, 2});
auto u = s1->set_union(*s2); // UnionSet
std::cout << u->toString(); // ([0, 1) U {1, 2})
```

## AST Parsing
All set string representations are designed to be parsed by a future AST parser, enabling symbolic manipulation and calculus operations. 