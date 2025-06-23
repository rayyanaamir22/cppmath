# compiler
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iinclude/doctest/doctest

all: test_matrix test_graph

test_matrix: tests/linalg/test_matrix.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test_graph: tests/graph/test_graph.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: all
	./test_matrix
	./test_graph

clean:
	rm -f test_matrix test_graph 