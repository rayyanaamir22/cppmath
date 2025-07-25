# cppmath Roadmap

This document outlines future directions and optimization ideas for the cppmath library.

## Unimplemented Concepts
- **Polynomial Rings**: Implement support for polynomial rings and their arithmetic.
- **Quotient Structures**: Add support for quotient groups, rings, and fields.
- **Module Theory**: Introduce modules over rings and related operations.
- **Galois Theory**: Implement Galois groups and field extensions.
- **Representation Theory**: Add basic support for group representations.
- **Category Theory**: Explore categorical abstractions for algebraic structures.
- **Sets as Domains and Codomains**: Use the new set classes to define the domain and codomain of mathematical functions, enabling symbolic and interval-based function definitions.
- **Expression Class and AST Parsing**: Implement an `Expression` class with a string representation that can be parsed by an AST parser. Set string representations will also be AST parsed. This will allow for symbolic manipulation of mathematical expressions and sets.

## Optimization Ideas
- **Efficient Operation Tables**: For finite algebraic structures (e.g., finite fields, groups), store the operation (e.g., multiplication, addition) tables as adjacency matrices or hashmaps. For example, the value at index (i, j) in the matrix or map would represent the result of composing the i-th and j-th elements. This can significantly speed up repeated operations at the cost of increased memory usage.
- **SIMD/Vectorization**: Use SIMD instructions for vector and matrix operations where possible.
- **Lazy Evaluation**: Implement lazy evaluation for expensive computations in algebraic structures.
- **Parallel Algorithms**: Explore parallelization for large-scale algebraic computations.

## Symbolic Calculus Framework
The set and expression string representations, designed for AST parsing, lay the groundwork for a future symbolic calculus framework. This will enable advanced symbolic manipulation, differentiation, integration, and function analysis using a unified AST-based approach.

## Progress
- Created a `src/core` folder with starter implementations for:
  - `Expression` (abstract base for symbolic expressions)
  - `Variable` (symbolic variable)
  - `Constant` (numeric constant)
  - `BinaryOperations` (Add, Multiply, Power)
  - `UnaryOperations` (Sin, Cos, Exp, Log)
  - `Function` (symbolic function object with domain/codomain as sets)
- These form the foundation for a symbolic system and future AST-based calculus engine. 