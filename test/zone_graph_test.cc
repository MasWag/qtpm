#include <boost/test/unit_test.hpp>
#include "../src/zone_graph.hh"
#include "../src/robustness.hh"
#include "../src/weighted_graph.hh"

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
  using Value = double;
  BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor> initStatesZG;

  zoneConstruction(TA, initStatesTA, ZG, initStatesZG);

  // TODO: write some tests
  BOOST_CHECK_EQUAL(num_vertices(ZG), 5);
  BOOST_CHECK_EQUAL(num_edges(ZG), 8);
}

BOOST_AUTO_TEST_CASE(zoneConstructionWithTTest0)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small2.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

  parseBoostTA(file, TA, initStatesTA);

  using Weight = MaxMinSemiring<double>;
  using Value = double;
  std::vector<BoostZoneGraphState<SignalVariables, ClockVariables, Value>> initConfTA;
  initConfTA.reserve(initStatesTA.size());
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_GT(num_of_vars, 0);

  auto zeroDBM = DBM::zero(num_of_vars + 2);
  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  zeroDBM.M = max_constraints;
  for (const auto &init: initStatesTA) {
    initConfTA.push_back({init, false, zeroDBM, std::vector<std::vector<Value>>{}});
  }

  BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor> initStatesZG;

  zoneConstructionWithT(TA, initConfTA, multipleSpaceRobustness<Weight, Value, ClockVariables>, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

  // TODO: write some tests
}

BOOST_AUTO_TEST_SUITE_END()
