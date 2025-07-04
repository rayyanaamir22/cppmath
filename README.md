# cppmath
A math library in C++, supporting numerous concepts I've learned as a math major :p

## Cloning this repository

This project uses git submodules (for doctest). To clone with all dependencies, use:

```sh
git clone --recurse-submodules <repo-url>
```

If you already cloned without submodules, run:

```sh
git submodule update --init --recursive
```

## Running Tests

To build and run all tests:

```sh
make test
```

To clean up test binaries:

```sh
make clean
```

Or, to run the test commands directly (without Makefile):

```sh
# Build and run Matrix tests
 g++ -std=c++17 -Iinclude -Iinclude/doctest/doctest -o test_matrix tests/linalg/test_matrix.cpp
 ./test_matrix

# Build and run Graph tests
 g++ -std=c++17 -Iinclude -Iinclude/doctest/doctest -o test_graph tests/graph/test_graph.cpp
 ./test_graph
```
