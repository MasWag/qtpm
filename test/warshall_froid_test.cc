#include <array>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/warshall_froid.hh"

BOOST_AUTO_TEST_SUITE(WarshallFroidTest)

typedef boost::mpl::list<MinPlusSemiring<double>, MaxMinSemiring<double>> testTypesInt;

template<class T>
struct ans_trait;

template<>
struct ans_trait<MinPlusSemiring<double>> {
  constexpr const static double ans = 2.0;
};

template<>
struct ans_trait<MaxMinSemiring<double>> {
  constexpr const static double ans = 3.0;
};

BOOST_AUTO_TEST_CASE_TEMPLATE( warshallFroidTest0, T, testTypesInt )
{
  using Graph = WeightedGraph<T>;
  Graph G;
  std::unordered_map<typename Graph::vertex_descriptor,
                     std::unordered_map<typename Graph::vertex_descriptor, T>> distance;
  std::array<typename Graph::vertex_descriptor, 4> vs;
  for (auto &v: vs) {
    v = add_vertex(G);
  }

  add_edge(vs[0], vs[2], T(-2), G);
  add_edge(vs[1], vs[0], T(4), G);
  add_edge(vs[1], vs[2], T(3), G);
  add_edge(vs[2], vs[3], T(2), G);
  add_edge(vs[3], vs[1], T(-1), G);

  warshall_froid(G, distance);

  constexpr auto const ans = ans_trait<T>::ans;

  BOOST_CHECK_EQUAL(distance[vs[1]][vs[2]].data, ans);
}

BOOST_AUTO_TEST_SUITE_END()
