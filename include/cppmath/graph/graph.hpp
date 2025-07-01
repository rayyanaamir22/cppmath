#pragma once
#include <vector>
#include <algorithm>

/**
 * @brief Graph algorithms and data structures.
 */
namespace cppmath {
    namespace graph {
    // Add graph function declarations here

        /**
         * @struct GraphNode
         * @brief Represents a vertex in the graph, with pointers to adjacent vertices.
         */
        struct GraphNode {
            size_t index; ///< Unique index of the node
            std::vector<GraphNode*> neighbors; ///< Pointers to adjacent nodes
        };

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

                /**
                 * @brief Returns a const reference to the vector of GraphNode objects.
                 */
                const std::vector<GraphNode>& nodes() const;

                /**
                 * @brief Returns a const reference to the adjacency matrix.
                 *        The matrix is a vector of vector<bool>.
                 */
                const std::vector<std::vector<bool>>& adjacency_matrix() const;

                /**
                 * @brief Returns a const reference to the adjacency list (vector of vectors of indices).
                 */
                const std::vector<std::vector<size_t>>& adjacency_list() const;

                /**
                 * @brief Checks if the graph is a tree (connected and acyclic).
                 * @return True if the graph is a tree, false otherwise.
                 */
                bool is_tree() const;
            private:
                std::vector<GraphNode> m_nodes; ///< List of all nodes
                std::vector<std::vector<size_t>> m_adj_list; ///< Adjacency list (indices)
                std::vector<std::vector<bool>> m_adj_matrix; ///< Adjacency matrix
                size_t m_edges;
        };
    }
}
