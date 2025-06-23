#include <iostream>
#include "cppmath/graph.hpp"
#include <cstddef>  // for size_t
#include <vector>   // for std::vector

using namespace cppmath::graph;

int main() {
    // Simulate a business case: offices and their direct communication links
    // Offices: 0 - HQ, 1 - BranchA, 2 - BranchB, 3 - BranchC
    Graph g;
    size_t hq = g.add_vertex();      // 0
    size_t branchA = g.add_vertex(); // 1
    size_t branchB = g.add_vertex(); // 2
    size_t branchC = g.add_vertex(); // 3

    // Add direct communication links (edges)
    g.add_edge(hq, branchA);   // HQ <-> BranchA
    g.add_edge(hq, branchB);   // HQ <-> BranchB
    g.add_edge(branchA, branchC); // BranchA <-> BranchC

    // Print adjacency list
    std::cout << "Adjacency List (by office index):\n";
    for (size_t i = 0; i < g.num_vertices(); ++i) {
        std::cout << "Office " << i << ": ";
        for (size_t neighbor : g.neighbors(i)) {
            std::cout << neighbor << " ";
        }
        std::cout << "\n";
    }

    // Print adjacency matrix
    std::cout << "\nAdjacency Matrix:\n   ";
    for (size_t i = 0; i < g.num_vertices(); ++i) std::cout << i << " ";
    std::cout << "\n";
    const auto& matrix = g.adjacency_matrix();
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << i << ": ";
        for (bool connected : matrix[i]) {
            std::cout << (connected ? "1" : "0") << " ";
        }
        std::cout << "\n";
    }

    // Use the node pointer API to print direct connections for each office
    std::cout << "\nDirect connections using GraphNode pointers:\n";
    const auto& nodes = g.nodes();
    for (const auto& node : nodes) {
        std::cout << "Office " << node.index << " is directly connected to: ";
        for (const auto* neighbor : node.neighbors) {
            std::cout << neighbor->index << " ";
        }
        std::cout << "\n";
    }

    // Simple business logic: check if HQ is directly connected to all branches
    bool hq_connected_to_all = true;
    for (size_t i = 1; i < g.num_vertices(); ++i) {
        if (!matrix[hq][i]) {
            hq_connected_to_all = false;
            break;
        }
    }
    std::cout << "\nHQ is " << (hq_connected_to_all ? "" : "NOT ") << "directly connected to all branches.\n";

    return 0;
} 