#include <boost/test/unit_test.hpp>
#include "../src/zone_graph.hh"

BOOST_AUTO_TEST_SUITE(zoneConstructionTest)

BOOST_AUTO_TEST_CASE(parseBoostTASimpleTest)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small2.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

  parseBoostTA(file, TA, initStatesTA);

  using Weight = double;
  BoostZoneGraph<SignalVariables, ClockVariables, Weight> ZG;
  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight>::vertex_descriptor> initStatesZG;

  zoneConstruction(TA, initStatesTA, ZG, initStatesZG);

  // TODO: write some tests
  BOOST_CHECK_EQUAL(num_vertices(ZG), 5);
  BOOST_CHECK_EQUAL(num_edges(ZG), 8);
}

BOOST_AUTO_TEST_SUITE_END()
