#include <boost/test/unit_test.hpp>
#include <array>
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

BOOST_AUTO_TEST_CASE(zoneConstructionWithTTest1)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small3.dot");
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

  BOOST_CHECK_EQUAL(initStatesTA.size(), 1);
  BOOST_CHECK_EQUAL(boost::num_vertices(TA), 3);
  const auto q0 = initStatesTA.front();
  BOOST_CHECK_EQUAL(boost::out_degree(q0, TA), 1);
  const auto q1 = target((*boost::out_edges(q0, TA).first), TA);
  BOOST_CHECK_EQUAL(boost::out_degree(q1, TA), 1);
  const auto q2 = target((*boost::out_edges(q1, TA).first), TA);
  BOOST_CHECK_EQUAL(boost::out_degree(q2, TA), 1);
  BOOST_CHECK_EQUAL(target((*boost::out_edges(q2, TA).first), TA), q0);

  initConfTA.reserve(2);
  initConfTA.push_back({q0, false, zeroDBM, std::vector<std::vector<Value>>{}});
  zeroDBM.elapse();
  const std::vector<std::vector<Value>> valuations = {{130, 20}, {150, 10}};
  initConfTA.push_back({q1, true,  zeroDBM, valuations});

  BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor> initStatesZG;

  zoneConstructionWithT(TA, initConfTA, multipleSpaceRobustness<Weight, Value, ClockVariables>, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

  // TODO: write some tests
  BOOST_CHECK_EQUAL(initStatesZG.size(), 2);
  BOOST_CHECK_EQUAL(boost::num_vertices(ZG), 8 + 7);

  const std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor, 2> expectedInitZGTAStates = {q0, q1};
  const std::array<bool, 2> expectedInitZGBs = {false, true};
  const std::array<std::size_t, 2> expectedInitZGValuationsSize = {0, 2};

  for (int i = 0; i < 2; i++) {
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].vertex, expectedInitZGTAStates[i]);
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].jumpable, expectedInitZGBs[i]);
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].valuations.size(), expectedInitZGValuationsSize[i]);
  }

}

BOOST_AUTO_TEST_CASE(zoneConstructionWithTForPatternMatchingTest1)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small3.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

  parseBoostTA(file, TA, initStatesTA);

  using Weight = MaxMinSemiring<double>;
  using Value = double;
  std::vector<BoostZoneGraphState<SignalVariables, ClockVariables, Value>> initConfTA;
  initConfTA.reserve(initStatesTA.size());
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_GT(num_of_vars, 0);

  auto zeroDBM = DBM::zero(num_of_vars + 3);
  auto DBMInitVar = num_of_vars;
  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  zeroDBM.M = std::numeric_limits<Value>::infinity();
  zeroDBM.release(DBMInitVar);

  BOOST_CHECK_EQUAL(initStatesTA.size(), 1);
  BOOST_CHECK_EQUAL(boost::num_vertices(TA), 3);
  const auto q0 = initStatesTA.front();
  BOOST_CHECK_EQUAL(boost::out_degree(q0, TA), 1);
  const auto q1 = target((*boost::out_edges(q0, TA).first), TA);
  BOOST_CHECK_EQUAL(boost::out_degree(q1, TA), 1);
  const auto q2 = target((*boost::out_edges(q1, TA).first), TA);
  BOOST_CHECK_EQUAL(boost::out_degree(q2, TA), 1);
  BOOST_CHECK_EQUAL(target((*boost::out_edges(q2, TA).first), TA), q0);

  initConfTA.reserve(2);
  initConfTA.push_back({q0, false, zeroDBM, std::vector<std::vector<Value>>{}});
  zeroDBM.elapse();
  const std::vector<std::vector<Value>> valuations = {{130, 20}, {150, 10}};
  initConfTA.push_back({q1, true,  zeroDBM, valuations});

  BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor> initStatesZG;

  zoneConstructionWithT(TA, initConfTA, multipleSpaceRobustness<Weight, Value, ClockVariables>, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

  // TODO: write some tests
  BOOST_CHECK_EQUAL(initStatesZG.size(), 2);
  BOOST_CHECK_EQUAL(boost::num_vertices(ZG), 8 + 7);

  const std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor, 2> expectedInitZGTAStates = {q0, q1};
  const std::array<bool, 2> expectedInitZGBs = {false, true};
  const std::array<std::size_t, 2> expectedInitZGValuationsSize = {0, 2};

  for (int i = 0; i < 2; i++) {
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].vertex, expectedInitZGTAStates[i]);
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].jumpable, expectedInitZGBs[i]);
    BOOST_CHECK_EQUAL(ZG[initStatesZG[i]].valuations.size(), expectedInitZGValuationsSize[i]);
  }

}

BOOST_AUTO_TEST_SUITE_END()
