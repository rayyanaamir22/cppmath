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

# Define groups - manually list tests for now (can be automated later)
TESTS = test_matrix test_graph test_root test_vector test_combo test_prime test_finite_field \
	test_signature test_expression test_traversal test_evaluate test_parser
# Source groups. core is linked as a whole: its pieces depend on each other (the
# printer behind Expression::toString, the Signature behind the builders, ...).
CORE_SRC = $(wildcard src/core/*.cpp) $(wildcard src/core/parse/*.cpp)
SETS_SRC = src/sets/IntervalSet.cpp src/sets/FiniteSet.cpp src/sets/UnionSet.cpp src/sets/IntersectionSet.cpp src/sets/EmptySet.cpp src/sets/UniversalSet.cpp

EXAMPLES = field_demo vector_demo group_demo ring_demo vsp_demo lie_demo set_demo matrix_demo graph_demo ml_pipeline

all: $(TESTS) $(EXAMPLES)

field_demo: examples/field_demo.cpp src/abstract_algebra/field/Field.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

vector_demo: examples/vector_demo.cpp src/linalg/vector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

group_demo: examples/group_demo.cpp src/abstract_algebra/group/Group.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

ring_demo: examples/ring_demo.cpp src/abstract_algebra/ring/Ring.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

vsp_demo: examples/vsp_demo.cpp src/abstract_algebra/vector/VectorSpace.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

lie_demo: examples/lie_demo.cpp src/abstract_algebra/lie/Lie.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_demo: examples/matrix_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

graph_demo: examples/graph_demo.cpp src/graph/graph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

ml_pipeline: examples/ml_pipeline.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

set_demo: examples/set_demo.cpp src/sets/IntervalSet.cpp src/sets/FiniteSet.cpp src/sets/UnionSet.cpp src/sets/IntersectionSet.cpp src/sets/EmptySet.cpp src/sets/UniversalSet.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TESTS)
	@for t in $(TESTS); do ./$$t; done

# Individual test rules
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

test_signature: tests/core/test_signature.cpp $(CORE_SRC) $(SETS_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_expression: tests/core/test_expression.cpp $(CORE_SRC) $(SETS_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_traversal: tests/core/test_traversal.cpp $(CORE_SRC) $(SETS_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_evaluate: tests/core/test_evaluate.cpp $(CORE_SRC) $(SETS_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_parser: tests/core/test_parser.cpp $(CORE_SRC) $(SETS_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TESTS) $(EXAMPLES) $(BENCHMARKS)

# Pattern rule for benchmarks (benchmarks/<subdir>/<file>.cpp -> benchmarks/<subdir>/<file>)
# Per-benchmark deps: add a rule below if a benchmark needs sources.
benchmarks/linalg/bench_vector_simd: benchmarks/linalg/bench_vector_simd.cpp src/linalg/vector.cpp
	$(CXX) $(BENCH_CXXFLAGS) -o $@ $^

benchmarks/numerics/bench_root_newton: benchmarks/numerics/bench_root_newton.cpp src/numerics/root.cpp
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