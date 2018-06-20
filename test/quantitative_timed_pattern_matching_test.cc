#include <queue>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include "../src/weighted_graph.hh"
#include "../src/robustness.hh"
#include "../src/quantitative_timed_pattern_matching.hh"

BOOST_AUTO_TEST_SUITE(QuantitativeTimedPatternMatchingTest)

BOOST_AUTO_TEST_CASE( QTPMTest0 )
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/pattern0.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStatesTA;

  parseBoostTA(file, TA, initStatesTA);

  using Weight = MaxMinSemiring<double>;
  using Value = double;
  std::vector<std::pair<BoostZoneGraphState<SignalVariables, ClockVariables, Value>, Weight>> initConfTA;
  initConfTA.reserve(initStatesTA.size());
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_EQUAL(num_of_vars, 0);

  std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost = multipleSpaceRobustness<Weight, Value, ClockVariables>;
  
  QuantitativeTimedPatternMatching<SignalVariables, ClockVariables, Weight, Value> qtpm(TA, initStatesTA, cost);

  std::vector<std::vector<Value>> valuations = {{150}, {60}};
  std::vector<double> durations = {0.1, 0.1};
  for (int i = 0; i < 1; i++) {
    qtpm.feed(valuations[i], durations[i]);
  }

  boost::unordered_map<decltype(qtpm)::ResultMatrix, Weight> result;
  qtpm.getResult(result);

  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(std::accumulate(result.begin(), result.end(), Weight::zero(),
                                    [](Weight init, std::pair<decltype(qtpm)::ResultMatrix, Weight> p) {
                                      return init + p.second.data;
                                    }).data, -80);
}

BOOST_AUTO_TEST_SUITE_END()
