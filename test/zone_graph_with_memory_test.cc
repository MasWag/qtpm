#include <boost/test/unit_test.hpp>
#include <array>
#include "../src/zone_graph_with_memory.hh"
#include "../src/robustness.hh"
#include "../src/weighted_graph.hh"

BOOST_AUTO_TEST_SUITE(ZoneGraphWithMemoryZoneConstructionTest)

BOOST_AUTO_TEST_CASE(zoneConstructionWithTTest0)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  using MemoryVariables = uint8_t;
  TSAM<SignalVariables, ClockVariables, MemoryVariables> TA;
  std::ifstream file("../test/dot/memories/small2.dot");
  std::vector<typename TSAMAutomaton<SignalVariables, ClockVariables, MemoryVariables>::vertex_descriptor> initStatesTA;

  parseTSAM(file, TA, initStatesTA);

  using Weight = MaxMinSemiring<double>;
  using Value = double;
  std::vector<std::pair<WZTSMState<SignalVariables, ClockVariables, Value>, Weight>> initConfTA;
  initConfTA.reserve(initStatesTA.size());
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_GT(num_of_vars, 0);

  auto zeroDBM = DBM::zero(num_of_vars + 1);
  zeroDBM.M = Bounds{std::numeric_limits<double>::infinity(), false};
  for (const auto &init: initStatesTA) {
    #error "from here!!"
    // TODO: parse size of memories
    initConfTA.emplace_back(WZTSMState<SignalVariables, ClockVariables, Value>{init, zeroDBM, std::vector<double>{}, std::vector<std::vector<Value>>{}}, Weight::one());
  }

  BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
  std::unordered_map<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor, Weight> initStatesZG;

  std::function<Weight(const std::vector<Constraint<ClockVariables>> &,
                       const std::vector<double> &
                       const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
  zoneConstructionWithT(TA, initConfTA, cost, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

  // TODO: write some tests
}

// BOOST_AUTO_TEST_CASE(zoneConstructionWithTTest1)
// {
//   using SignalVariables = uint8_t;
//   using ClockVariables = uint8_t;
//   BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
//   std::ifstream file("../test/small3.dot");
//   std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

//   parseBoostTA(file, TA, initStatesTA);

//   using Weight = MaxMinSemiring<double>;
//   using Value = double;
//   std::vector<std::pair<BoostZoneGraphState<SignalVariables, ClockVariables, Value>, Weight>> initConfTA;
//   initConfTA.reserve(initStatesTA.size());
//   const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
//   BOOST_CHECK_GT(num_of_vars, 0);

//   auto zeroDBM = DBM::zero(num_of_vars + 2);
//   const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
//   auto DBMDwellTimeVar = num_of_vars + 1 - 1;

//   zeroDBM.M = Bounds{max_constraints, true};
//   zeroDBM.tighten(-1, DBMDwellTimeVar, {0, true});
//   zeroDBM.release(DBMDwellTimeVar);

//   BOOST_CHECK_EQUAL(initStatesTA.size(), 1);
//   BOOST_CHECK_EQUAL(boost::num_vertices(TA), 3);
//   const auto q0 = initStatesTA.front();
//   BOOST_CHECK_EQUAL(boost::out_degree(q0, TA), 1);
//   const auto q1 = target((*boost::out_edges(q0, TA).first), TA);
//   BOOST_CHECK_EQUAL(boost::out_degree(q1, TA), 1);
//   const auto q2 = target((*boost::out_edges(q1, TA).first), TA);
//   BOOST_CHECK_EQUAL(boost::out_degree(q2, TA), 1);
//   BOOST_CHECK_EQUAL(target((*boost::out_edges(q2, TA).first), TA), q0);

//   initConfTA.reserve(2);
//   initConfTA.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q0, false, zeroDBM, std::vector<std::vector<Value>>{}}, Weight::one());
//   zeroDBM.elapse();
//   const std::vector<std::vector<Value>> valuations = {{130, 20}, {150, 10}};
//   initConfTA.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q1, true,  zeroDBM, valuations}, Weight::one());

//   BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
//   std::unordered_map<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor, Weight> initStatesZG;

//   std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
//   zoneConstructionWithT(TA, initConfTA, cost, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

//   // TODO: write some tests
//   BOOST_CHECK_EQUAL(initStatesZG.size(), 2);
//   BOOST_CHECK_EQUAL(boost::num_vertices(ZG), 8 + 7);

//   // const std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor, 2> expectedInitZGTAStates = {{q0, q1}};
//   // const std::array<bool, 2> expectedInitZGBs = {{false, true}};
//   // const std::array<std::size_t, 2> expectedInitZGValuationsSize = {{0, 2}};

//   // for (int i = 0; i < 2; i++) {
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].vertex, expectedInitZGTAStates[i]);
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].jumpable, expectedInitZGBs[i]);
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].valuations.size(), expectedInitZGValuationsSize[i]);
//   // }

// }

// BOOST_AUTO_TEST_CASE(zoneConstructionWithTForPatternMatchingTest1)
// {
//   using SignalVariables = uint8_t;
//   using ClockVariables = uint8_t;
//   BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
//   std::ifstream file("../test/small3.dot");
//   std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

//   parseBoostTA(file, TA, initStatesTA);

//   using Weight = MaxMinSemiring<double>;
//   using Value = double;
//   std::vector<std::pair<BoostZoneGraphState<SignalVariables, ClockVariables, Value>, Weight>> initConfTA;
//   initConfTA.reserve(initStatesTA.size());
//   const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
//   BOOST_CHECK_GT(num_of_vars, 0);

//   auto zeroDBM = DBM::zero(num_of_vars + 3);
//   auto DBMDwellTimeVar = num_of_vars + 2 - 1;

//   zeroDBM.M = Bounds{std::numeric_limits<Value>::infinity(), false};
//   zeroDBM.tighten(-1, DBMDwellTimeVar, {0, true});
//   zeroDBM.release(DBMDwellTimeVar);

//   BOOST_CHECK_EQUAL(initStatesTA.size(), 1);
//   BOOST_CHECK_EQUAL(boost::num_vertices(TA), 3);
//   const auto q0 = initStatesTA.front();
//   BOOST_CHECK_EQUAL(boost::out_degree(q0, TA), 1);
//   const auto q1 = target((*boost::out_edges(q0, TA).first), TA);
//   BOOST_CHECK_EQUAL(boost::out_degree(q1, TA), 1);
//   const auto q2 = target((*boost::out_edges(q1, TA).first), TA);
//   BOOST_CHECK_EQUAL(boost::out_degree(q2, TA), 1);
//   BOOST_CHECK_EQUAL(target((*boost::out_edges(q2, TA).first), TA), q0);

//   initConfTA.reserve(2);
//   initConfTA.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q0, false, zeroDBM, std::vector<std::vector<Value>>{}}, Weight::one());
//   zeroDBM.elapse();
//   const std::vector<std::vector<Value>> valuations = {{130, 20}, {150, 10}};
//   initConfTA.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q1, true,  zeroDBM, valuations}, Weight::one());

//   BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> ZG;
//   std::unordered_map<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor, Weight> initStatesZG;

//   std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
//   zoneConstructionWithT(TA, initConfTA, cost, std::vector<Value>{20, 30}, 3.0, ZG, initStatesZG);

//   // TODO: write some tests
//   BOOST_CHECK_EQUAL(initStatesZG.size(), 2);
//   BOOST_CHECK_EQUAL(boost::num_vertices(ZG), 8 + 7);

//   // const std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor, 2> expectedInitZGTAStates = {{q0, q1}};
//   // const std::array<bool, 2> expectedInitZGBs = {{false, true}};
//   // const std::array<std::size_t, 2> expectedInitZGValuationsSize = {{0, 2}};

//   // for (int i = 0; i < 2; i++) {
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].vertex, expectedInitZGTAStates[i]);
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].jumpable, expectedInitZGBs[i]);
//   //   BOOST_CHECK_EQUAL(ZG[initStatesZG[i].first].valuations.size(), expectedInitZGValuationsSize[i]);
//   // }

// }

BOOST_AUTO_TEST_SUITE_END()
