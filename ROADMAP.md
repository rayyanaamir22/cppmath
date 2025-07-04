# cppmath Roadmap

This document outlines future directions and optimization ideas for the cppmath library.

## Unimplemented Concepts
- **Polynomial Rings**: Implement support for polynomial rings and their arithmetic.
- **Quotient Structures**: Add support for quotient groups, rings, and fields.
- **Module Theory**: Introduce modules over rings and related operations.
- **Galois Theory**: Implement Galois groups and field extensions.
- **Representation Theory**: Add basic support for group representations.
- **Category Theory**: Explore categorical abstractions for algebraic structures.

## Optimization Ideas
- **Efficient Operation Tables**: For finite algebraic structures (e.g., finite fields, groups), store the operation (e.g., multiplication, addition) tables as adjacency matrices or hashmaps. For example, the value at index (i, j) in the matrix or map would represent the result of composing the i-th and j-th elements. This can significantly speed up repeated operations at the cost of increased memory usage.
- **SIMD/Vectorization**: Use SIMD instructions for vector and matrix operations where possible.
- **Lazy Evaluation**: Implement lazy evaluation for expensive computations in algebraic structures.
- **Parallel Algorithms**: Explore parallelization for large-scale algebraic computations. 