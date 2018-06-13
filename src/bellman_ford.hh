#include <unordered_map>

#include "weighted_graph.hh"

template<typename Queue, typename WeightedGraph>
void bellman_ford(const WeightedGraph &G, const typename WeightedGraph::vertex_descriptor source,
                  std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type> &distance) {
  std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type> r;
  for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
    auto i = *range.first;
    distance[i] = WeightedGraph::edge_property_type::value_type::zero(); 
    r[i] = WeightedGraph::edge_property_type::value_type::zero(); 
  }
  distance[source] = WeightedGraph::edge_property_type::value_type::one();
  r[source] = WeightedGraph::edge_property_type::value_type::one();
  
  auto edgeWeightMap = get(boost::edge_weight, G);

  Queue Q;
  Q.push(source);
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
