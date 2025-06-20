#pragma once

/**
 * @brief Graph algorithms and data structures.
 */
namespace cppmath {
    namespace graph {
    // Add graph function declarations here

        /**
         * @class Graph
         * @brief Represents an undirected, unweighted graph using an adjacency list.
         *
         * This class provides basic graph operations such as adding vertices and edges,
         * and querying neighbors. The design is extendible for weights, labels, or algorithms.
         */
        class Graph {
            public:
                /**
                 * @brief Constructs an empty graph.
                 */
                Graph();

                /**
                 * @brief Adds a new vertex to the graph.
                 * @return The index of the newly added vertex.
                 */
                size_t add_vertex();

                /**
                 * @brief Adds an undirected edge between vertices u and v.
                 * @param u Index of the first vertex.
                 * @param v Index of the second vertex.
                 *
                 * If either vertex does not exist, the function does nothing.
                 */
                void add_edge(size_t u, size_t v);

                /**
                 * @brief Returns a const reference to the neighbors of vertex v.
                 * @param v Index of the vertex.
                 * @return A const reference to a vector of neighbor indices.
                 */
                const std::vector<size_t>& neighbors(size_t v) const;

                /**
                 * @brief Returns the number of vertices in the graph.
                 */
                size_t num_vertices() const;

                /**
                 * @brief Returns the number of edges in the graph.
                 */
                size_t num_edges() const;
            private:
                /**
                 * @brief Adjacency list: m_adj[i] contains the neighbors of vertex i.
                 */
                std::vector<std::vector<size_t>> m_adj;

                /**
                 * @brief The total number of edges in the graph.
                 */
                size_t m_edges;
        };
    }
}
