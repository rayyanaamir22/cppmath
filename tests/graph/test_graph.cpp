#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/graph.hpp"

TEST_CASE("Graph vertex and edge operations") {
    cppmath::graph::Graph g;
    size_t v0 = g.add_vertex();
    size_t v1 = g.add_vertex();
    size_t v2 = g.add_vertex();
    CHECK(g.num_vertices() == 3);
    g.add_edge(v0, v1);
    g.add_edge(v1, v2);
    CHECK(g.num_edges() == 2);
    // Check neighbors
    auto n0 = g.neighbors(v0);
    auto n1 = g.neighbors(v1);
    auto n2 = g.neighbors(v2);
    CHECK(n0.size() == 1);
    CHECK(n0[0] == v1);
    CHECK(n1.size() == 2);
    CHECK((n1[0] == v0 || n1[0] == v2));
    CHECK((n1[1] == v0 || n1[1] == v2));
    CHECK(n2.size() == 1);
    CHECK(n2[0] == v1);
    // Check adjacency matrix
    const auto& mat = g.adjacency_matrix();
    CHECK(mat[v0][v1] == true);
    CHECK(mat[v1][v0] == true);
    CHECK(mat[v1][v2] == true);
    CHECK(mat[v2][v1] == true);
    CHECK(mat[v0][v2] == false);
    CHECK(mat[v2][v0] == false);
} 