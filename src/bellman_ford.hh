#include <unordered_map>

#include "weighted_graph.hh"

/*!
  @brief Semiring-based (single source) shortest path problem
  @param [in] G A weighted automaton.
  @param [in] init Initial cost of the vertices. For the vertices not in the list, the initial cost is one.
  @param [out] distance The shortest cost from the initial cost.
 */
template<typename Queue, typename WeightedGraph>
void bellman_ford(const WeightedGraph &G, 
                  const std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type> &init,
                  std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type> &distance) {
  std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type> r;
  for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
    auto i = *range.first;
    distance[i] = WeightedGraph::edge_property_type::value_type::zero(); 
    r[i] = WeightedGraph::edge_property_type::value_type::zero(); 
  }
  Queue Q;
  for (const auto &q: init) {
    distance[q.first] = q.second;
    r[q.first] = q.second;
    Q.push(q.first);
  }
  
  auto edgeWeightMap = get(boost::edge_weight, G);

  while (!Q.empty()) {
    auto q = Q.front();
    Q.pop();
    const auto currentR = r[q];
    r[q] = WeightedGraph::edge_property_type::value_type::zero();

    for (auto range = boost::out_edges(q, G); range.first != range.second; range.first++) {
      auto e = *range.first;
      if (distance[target(e, G)] != distance[target(e, G)] + (currentR * edgeWeightMap[e])) {
        distance[target(e, G)] += (currentR * edgeWeightMap[e]);
        r[target(e, G)] += (currentR * edgeWeightMap[e]);
        Q.push(target(e, G));
      }
    }
  }
}
