#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cppmath/graph/graph.hpp"

TEST_CASE("Basic Operations") {
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

TEST_CASE("Adjacency Matrix and List: Individual and Cross-Validation") {
    cppmath::graph::Graph g;
    size_t v0 = g.add_vertex();
    size_t v1 = g.add_vertex();
    size_t v2 = g.add_vertex();
    g.add_edge(v0, v1);
    g.add_edge(v1, v2);
    g.add_edge(v2, v0);
    // Individual checks
    const auto& mat = g.adjacency_matrix();
    const auto& list = g.adjacency_list();
    CHECK(mat[v0][v1] == true);
    CHECK(mat[v1][v2] == true);
    CHECK(mat[v2][v0] == true);
    CHECK(list[v0].size() == 2);
    CHECK(list[v1].size() == 2);
    CHECK(list[v2].size() == 2);
    // Cross-validation
    for (size_t u = 0; u < g.num_vertices(); ++u) {
        for (size_t v = 0; v < g.num_vertices(); ++v) {
            bool in_list = std::find(list[u].begin(), list[u].end(), v) != list[u].end();
            CHECK(mat[u][v] == in_list);
        }
    }
}

TEST_CASE("Circle Graph Properties (C_n)") {
    for (int n : {3, 5, 10, 20}) { // try C_n for these positive integers n
        cppmath::graph::Graph g;
        std::vector<size_t> verts;
        for (int i = 0; i < n; ++i) verts.push_back(g.add_vertex());
        for (int i = 0; i < n; ++i) g.add_edge(verts[i], verts[(i+1)%n]);
        CHECK(g.num_vertices() == (size_t)n);
        CHECK(g.num_edges() == (size_t)n);
        const auto& mat = g.adjacency_matrix();
        const auto& list = g.adjacency_list();
        for (int i = 0; i < n; ++i) {
            // Each vertex has two neighbors
            CHECK(list[i].size() == 2);
            CHECK(mat[i][(i+1)%n] == true);
            CHECK(mat[i][(i-1+n)%n] == true);
            // No self-loops
            CHECK(mat[i][i] == false);
        }
    }
}

TEST_CASE("Empty Graph") {
    cppmath::graph::Graph g;
    CHECK(g.num_vertices() == 0);
    CHECK(g.num_edges() == 0);
    CHECK(g.adjacency_matrix().size() == 0);
    CHECK(g.adjacency_list().size() == 0);
}

TEST_CASE("Single Vertex Graph") {
    cppmath::graph::Graph g;
    size_t v = g.add_vertex();
    CHECK(g.num_vertices() == 1);
    CHECK(g.num_edges() == 0);
    CHECK(g.adjacency_list()[v].empty());
    CHECK(g.adjacency_matrix()[v][v] == false);
}

TEST_CASE("Self-loop and Duplicate Edge Prevention") {
    cppmath::graph::Graph g;
    size_t v0 = g.add_vertex();
    size_t v1 = g.add_vertex();
    g.add_edge(v0, v0); // Should not add
    CHECK(g.num_edges() == 0);
    g.add_edge(v0, v1);
    CHECK(g.num_edges() == 1);
    g.add_edge(v1, v0); // Duplicate, should not increase
    CHECK(g.num_edges() == 1);
}

