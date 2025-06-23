/**
 * @file graph.cpp
 * @brief Graph algorithms implementation.
 */
#include "cppmath/graph/graph.hpp"

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
            size_t idx = m_nodes.size();
            m_nodes.push_back(GraphNode{idx, {}});
            m_adj_list.emplace_back();
            // Expand adjacency matrix
            for (auto& row : m_adj_matrix) {
                row.push_back(false);
            }
            m_adj_matrix.emplace_back(m_nodes.size(), false);
            return idx;
        }

        // Adds an undirected edge between vertices u and v.
        // If either vertex does not exist, the function does nothing.
        void Graph::add_edge(size_t u, size_t v) {
            if (u >= m_nodes.size() || v >= m_nodes.size() || u == v) return;
            // Avoid duplicate edges in adjacency list
            auto& adj_u = m_adj_list[u];
            auto& adj_v = m_adj_list[v];
            if (std::find(adj_u.begin(), adj_u.end(), v) == adj_u.end()) {
                adj_u.push_back(v);
                m_nodes[u].neighbors.push_back(&m_nodes[v]);
            }
            if (std::find(adj_v.begin(), adj_v.end(), u) == adj_v.end()) {
                adj_v.push_back(u);
                m_nodes[v].neighbors.push_back(&m_nodes[u]);
            }
            // Set adjacency matrix
            if (!m_adj_matrix[u][v]) {
                m_adj_matrix[u][v] = true;
                m_adj_matrix[v][u] = true;
                ++m_edges;
            }
        }

        // Returns a const reference to the neighbors of vertex v (as indices).
        const std::vector<size_t>& Graph::neighbors(size_t v) const {
            return m_adj_list[v];
        }

        // Returns the number of vertices in the graph.
        size_t Graph::num_vertices() const {
            return m_nodes.size();
        }

        // Returns the number of edges in the graph.
        size_t Graph::num_edges() const {
            return m_edges;
        }

        // Returns a const reference to the vector of GraphNode objects.
        const std::vector<GraphNode>& Graph::nodes() const {
            return m_nodes;
        }

        // Returns a const reference to the adjacency matrix.
        const std::vector<std::vector<bool>>& Graph::adjacency_matrix() const {
            return m_adj_matrix;
        }

        // Returns a const reference to the adjacency list.
        const std::vector<std::vector<size_t>>& Graph::adjacency_list() const {
            return m_adj_list;
        }

        // Add graph algorithm implementations here
    }
} 