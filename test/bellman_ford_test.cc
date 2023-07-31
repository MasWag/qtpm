#include <array>
#include <queue>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/bellman_ford.hh"

BOOST_AUTO_TEST_SUITE(BellmanFordTest)

typedef boost::mpl::list<MinPlusSemiring<double>, MaxMinSemiring<int>> testTypesInt;

template<class T>
struct ans_trait;

template<>
struct ans_trait<MinPlusSemiring<double>> {
  const static int ans = 2;
};

template<>
struct ans_trait<MaxMinSemiring<int>> {
  const static int ans = 3;
};

BOOST_AUTO_TEST_CASE_TEMPLATE( bellmanFordTest0, T, testTypesInt )
{
  using Graph = WeightedGraph<T>;
  Graph G;
  std::unordered_map<typename Graph::vertex_descriptor, T> distance;
  std::array<typename Graph::vertex_descriptor, 4> vs;
  for (auto &v: vs) {
    v = add_vertex(G);
  }

  add_edge(vs[0], vs[2], T(-2), G);
  add_edge(vs[1], vs[0], T(4), G);
  add_edge(vs[1], vs[2], T(3), G);
  add_edge(vs[2], vs[3], T(2), G);
  add_edge(vs[3], vs[1], T(-1), G);

  std::unordered_map<typename Graph::vertex_descriptor, typename Graph::edge_property_type::value_type> init;
  init[vs[1]] = T::one();
  
  bellman_ford<std::queue<typename Graph::vertex_descriptor>>(G, init, distance);

  constexpr int const ans = ans_trait<T>::ans;

  BOOST_CHECK_EQUAL(distance[vs[2]].data, ans);
}

BOOST_AUTO_TEST_SUITE_END()
