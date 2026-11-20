# graph plan

_Last updated 2026-11-20 · status: **early** · scope beyond the fixes is *proposed*_

Graph theory: graphs as vertices and edges, their traversal, connectivity, paths, trees and colourings, and their link to linear algebra through adjacency and Laplacian matrices.

## Status

- `graph.hpp`: `Graph` (undirected, unweighted) with `add_vertex`, `add_edge`, `neighbors`, `num_vertices`, `num_edges`, `is_tree`; `test_graph`, `graph_demo`.
- **Storage:** each graph keeps the same edges three times: an adjacency list (`m_adj_list`), an adjacency matrix (`m_adj_matrix`), and `GraphNode` objects holding raw pointers to their neighbours.

**Known bugs**
- **Dangling pointers.** `GraphNode::neighbors` stores `&m_nodes[v]`, pointers into a `std::vector`. Adding a vertex after any edge can reallocate `m_nodes`, leaving every stored pointer dangling.
- **Silent errors.** `add_edge` silently ignores self-loops and out-of-range vertices instead of throwing (`std::out_of_range` / `std::invalid_argument` per CLAUDE.md).
- **`is_tree()` is unimplemented:** it throws `std::logic_error("not implemented")`, and `test_graph` asserts that it throws. (The old root PLAN listed `is_tree` as done; it isn't.)
- **Memory:** the adjacency matrix makes every graph O(V²) in memory, even sparse ones.

## Roadmap

- [ ] **1. One representation.** Remove `GraphNode` and its pointers; make the adjacency list the source of truth; make the adjacency matrix an on-demand view (or optional). `add_edge` validates its arguments and throws. *Tests:* adding vertices after edges keeps neighbours correct (the dangling-pointer case); invalid edges throw.
- [ ] **2. Traversal and structure.** BFS and DFS (visitor or callback), connected components, cycle detection, then implement `is_tree` (connected and |E| = |V| − 1) and change its tests to check values.
- [ ] **3. Variants** *(proposed)*: directed graphs and weighted edges, as a template parameter or separate classes (decision below). Topological sort and strongly connected components for directed graphs.
- [ ] **4. Algorithms** *(proposed)*:
  - Shortest paths: BFS (unweighted), Dijkstra, Bellman–Ford (with negative-cycle detection), Floyd–Warshall.
  - Minimum spanning tree: Kruskal (with a union-find) and Prim.
  - Bipartiteness, greedy colouring.
  - Euler paths and circuits.
- [ ] **5. Spectral view** *(proposed)*: adjacency and Laplacian matrices as `linalg::Matrix`; the spanning-tree count via Kirchhoff's matrix-tree theorem (a determinant, so it needs linalg's LU); eigenvalues once linalg has them.

## Performance

- A compressed sparse row (CSR) layout for large, static graphs: cache-friendly traversal. Benchmark BFS on CSR against the adjacency list.
- Parallel BFS (level-synchronous) as an experiment for large graphs.

## Connections

- **Needs:** [linalg](../linalg/plan.md) (matrices, determinants, eigenvalues), [combinatorics](../combinatorics/plan.md) (counting).
- **Offers:** examples of matrices for [linalg](../linalg/plan.md) benchmarks; graphs of groups (Cayley graphs) for [abstract_algebra](../abstract_algebra/plan.md).

## Decisions needed

- **Directed/weighted design:** one `Graph<Directed, Weight>` template, or separate classes?
- **Adjacency matrix:** keep it as an optional cached view, or drop it and build `linalg::Matrix` on demand?
- **Vertex labels:** indices only (as now), or arbitrary vertex values?
