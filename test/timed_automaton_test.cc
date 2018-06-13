#include <boost/test/unit_test.hpp>
#include "../src/timed_automaton.hh"

BOOST_AUTO_TEST_SUITE(timedAutomatonParserTests)
BOOST_AUTO_TEST_CASE(parseBoostPhi7TATest)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/phi7.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_EQUAL(max_constraints, 4);
  BOOST_CHECK_EQUAL(num_of_vars, 1);

  BOOST_TEST(!TA[0].isMatch);
  BOOST_TEST(!TA[1].isMatch);
  BOOST_TEST( TA[2].isMatch);
  BOOST_TEST( TA[0].isInit);
  BOOST_TEST(!TA[1].isInit);
  BOOST_TEST(!TA[2].isInit);
  auto transition = boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 1);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars[0], 0);

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 0);
}

BOOST_AUTO_TEST_CASE(parseBoostTATest)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/timed_automaton.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  BOOST_TEST(!TA[0].isMatch);
  BOOST_TEST( TA[1].isMatch);
  BOOST_TEST( TA[0].isInit);
  BOOST_TEST(!TA[1].isInit);
  auto transition = boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 2);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars[0], 0);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars[1], 1);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 2);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[0].x, 0);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[0].c, 1);
  BOOST_TEST((boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[0].odr == Constraint<ClockVariables>::Order::gt));
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[1].x, 2);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[1].c, 10);
  BOOST_TEST((boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition)[1].odr == Constraint<ClockVariables>::Order::lt));
}

BOOST_AUTO_TEST_CASE(parseBoostTASimpleTest)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  std::array<bool, 4> initResult  = {{true, false, false, false}};
  std::array<bool, 4> matchResult = {{false, false, false, true}};

  for (int i = 0; i < 4; i++) {
    BOOST_CHECK_EQUAL(TA[i].isInit,  initResult[i]);
    BOOST_CHECK_EQUAL(TA[i].isMatch, matchResult[i]);
  }

  std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::edge_descriptor, 4> transitions = {{
      boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first,
      boost::edge(boost::vertex(1, TA), boost::vertex(2, TA), TA).first,
      boost::edge(boost::vertex(2, TA), boost::vertex(1, TA), TA).first,
      boost::edge(boost::vertex(2, TA), boost::vertex(3, TA), TA).first
    }};

  std::array<std::size_t, 4> resetVarNumResult  = {{0, 0, 0, 0}};
  std::array<std::size_t, 4> guardNumResult  = {{1, 1, 1, 1}};
  std::array<int, 4> guardXResult  = {{0, 0, 0, 0}};
  std::array<int, 4> guardCResult  = {{1, 1, 1, 1}};
  std::array<Constraint<ClockVariables>::Order, 4> guardOdrResult  = {{Constraint<ClockVariables>::Order::lt,
                                                                       Constraint<ClockVariables>::Order::lt, 
                                                                       Constraint<ClockVariables>::Order::lt, 
                                                                       Constraint<ClockVariables>::Order::lt}};

  for (int i = 0; i < 4; i++) {
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transitions[i]).resetVars.size(), resetVarNumResult[i]);
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i]).size(), guardNumResult[i]);
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].x, guardXResult[i]);
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].c, guardCResult[i]);
    BOOST_TEST((boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].odr == guardOdrResult[i]));
  }
}

BOOST_AUTO_TEST_SUITE_END()
