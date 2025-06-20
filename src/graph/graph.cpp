/**
 * @file graph.cpp
 * @brief Graph algorithms implementation.
 */
#include "cppmath/graph.hpp"

namespace cppmath {
    namespace graph {

        class Graph;

        // Implementation of the Graph class declared in graph.hpp
        // This class uses an adjacency list to represent an undirected, unweighted graph.

        // Constructor: Initializes an empty graph with zero edges.
        Graph::Graph() : m_edges(0) {}

        // Adds a new vertex to the graph.
        // Returns the index of the newly added vertex.
        size_t Graph::add_vertex() {
            m_adj.emplace_back(); // Add an empty neighbor list for the new vertex
            return m_adj.size() - 1; // Return the index of the new vertex
        }

        // Adds an undirected edge between vertices u and v.
        // If either vertex does not exist, the function does nothing.
        void Graph::add_edge(size_t u, size_t v) {
            // Check for valid vertex indices
            if (u >= m_adj.size() || v >= m_adj.size()) return;

            m_adj[u].push_back(v); // Add v to u's neighbor list
            m_adj[v].push_back(u); // Add u to v's neighbor list (undirected)

            // increment edge count
            ++m_edges;
        }

        // Returns a const reference to the neighbors of vertex v.
        const std::vector<size_t>& Graph::neighbors(size_t v) const {
            return m_adj[v];
        }

        // Returns the number of vertices in the graph.
        size_t Graph::num_vertices() const {
            return m_adj.size();
        }

        // Returns the number of edges in the graph.
        size_t Graph::num_edges() const {
            return m_edges;
        }

        // Add graph algorithm implementations here
    }
} 