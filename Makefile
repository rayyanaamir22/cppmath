# compiler
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iinclude/doctest/doctest

all: test_matrix test_graph test_root test_vector test_combo test_prime vector_demo

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

vector_demo: examples/vector_demo.cpp src/linalg/vector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: all
	./test_matrix
	./test_graph
	./test_root
	./test_vector
	./test_combo
	./test_prime

clean:
	rm -f test_matrix test_graph test_root test_vector test_combo test_prime vector_demo 