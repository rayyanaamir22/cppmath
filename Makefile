# compiler
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iinclude/doctest/doctest

all: test_matrix test_graph test_solver test_vector

test_matrix: tests/linalg/test_matrix.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_graph: tests/graph/test_graph.cpp src/graph/graph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_root: tests/numerics/test_root.cpp src/numerics/root.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_vector: tests/linalg/test_vector.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: all
	./test_matrix
	./test_graph
	./test_root
	./test_vector

clean:
	rm -f test_matrix test_graph test_root test_vector 