# compiler
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iinclude/doctest/doctest

# Benchmark build flags (add AVX for SIMD on x86_64 only)
ARCH := $(shell uname -m)
ifeq ($(ARCH),x86_64)
    BENCH_CXXFLAGS = $(CXXFLAGS) -O3 -mavx
else
    BENCH_CXXFLAGS = $(CXXFLAGS) -O3
endif
BENCHMARKS := $(patsubst benchmarks/%.cpp,benchmarks/%,$(wildcard benchmarks/*/*.cpp))

all: test_matrix test_graph test_root test_vector test_combo test_prime test_finite_field vector_demo

field_demo: examples/field_demo.cpp src/abstract_algebra/field/Field.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

vector_demo: examples/vector_demo.cpp src/linalg/vector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_matrix: tests/linalg/test_matrix.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_graph: tests/graph/test_graph.cpp src/graph/graph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_root: tests/numerics/test_root.cpp src/numerics/root.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_vector: tests/linalg/test_vector.cpp src/linalg/vector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_combo: tests/combinatorics/test_combo.cpp src/combinatorics/combo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_prime: tests/number_theory/test_prime.cpp src/number_theory/prime.cpp src/numerics/root.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_finite_field: tests/abstract_algebra/field/test_finite_field.cpp src/abstract_algebra/field/Field.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: all
	./test_matrix
	./test_graph
	./test_root
	./test_vector
	./test_combo
	./test_prime
	./test_finite_field

clean:
	rm -f test_matrix test_graph test_root test_vector test_combo test_prime vector_demo test_finite_field 

# Pattern rule for benchmarks (benchmarks/<subdir>/<file>.cpp -> benchmarks/<subdir>/<file>)
benchmarks/%: benchmarks/%.cpp src/linalg/vector.cpp
	$(CXX) $(BENCH_CXXFLAGS) -o $@ $^

# Build all benchmarks
benchmarks: $(BENCHMARKS)

# Run all benchmarks
run-benchmarks: benchmarks
	@for bm in $(BENCHMARKS); do \
		echo "Running $$bm:"; \
		./$$bm; \
	echo "----------------------"; \
	done

# Usage:
#   make benchmarks        # Build all benchmarks in benchmarks/*/*.cpp
#   make run-benchmarks    # Build and run all benchmarks 