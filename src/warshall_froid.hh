#include <unordered_map>

#include <boost/graph/adjacency_list.hpp>

#include "weighted_graph.hh"

/*!
  @brief Semiring-based (all source) shortest path problem
  @param [in] G A weighted graph.
  @param [out] distance The shortest cost from the initial cost.
 */
template<typename WeightedGraph>
void warshall_froid(const WeightedGraph &G, 
                    std::unordered_map<typename WeightedGraph::vertex_descriptor, 
                    std::unordered_map<typename WeightedGraph::vertex_descriptor, typename WeightedGraph::edge_property_type::value_type>> &distance) {
  for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
    auto i = *range.first;
    for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
      auto j = *range.first;
      distance[i][j] = WeightedGraph::edge_property_type::value_type::zero(); 
    }
  }

  auto edgeWeightMap = get(boost::edge_weight, G);
  for (auto range = boost::edges(G); range.first != range.second; range.first++) {
    auto e = *range.first;
    distance[source(e, G)][target(e, G)] += edgeWeightMap[e];
  }

  for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
    auto k = *range.first;
    for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
      auto i = *range.first;
      if (k == i) { 
        continue;
      }
      for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
        auto j = *range.first;
        if (k == j) { 
          continue;
        }
        distance[i][j] += (distance[i][k] * 
                           distance[k][k].star() * 
                           distance[k][j]);
      }
    }
    for (auto range = boost::vertices(G); range.first != range.second; range.first++) {
      auto i = *range.first;
      if (k == i) { 
        continue;
      }
      distance[k][i] = distance[k][k].star() * distance[k][i];
      distance[i][k] = distance[i][k] * distance[k][k].star();
    }
    distance[k][k] = distance[k][k].star();
  }
}
