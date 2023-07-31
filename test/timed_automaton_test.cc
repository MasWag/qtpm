#include <array>

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
  BOOST_CHECK_EQUAL(TA[0].label.size(), 2);
  BOOST_CHECK_EQUAL(TA[1].label.size(), 1);
  BOOST_CHECK_EQUAL(TA[2].label.size(), 0);
  auto transition = boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 1);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars[0], 0);

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 0);
}

BOOST_AUTO_TEST_CASE(parseVelocityBoostTATest)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/velocity.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  BOOST_CHECK_EQUAL(max_constraints, 200);
  BOOST_CHECK_EQUAL(num_of_vars, 1);

  BOOST_TEST(!TA[3].isMatch);
  BOOST_TEST(!TA[1].isMatch);
  BOOST_TEST(!TA[2].isMatch);
  BOOST_TEST( TA[0].isMatch);
  
  BOOST_TEST( TA[3].isInit);
  BOOST_TEST( TA[1].isInit);
  BOOST_TEST(!TA[2].isInit);
  BOOST_TEST(!TA[0].isInit);

  BOOST_CHECK_EQUAL(TA[3].label.size(), 1);
  BOOST_CHECK_EQUAL(TA[1].label.size(), 1);
  BOOST_CHECK_EQUAL(TA[2].label.size(), 1);
  BOOST_CHECK_EQUAL(TA[0].label.size(), 0);
  auto transition = boost::edge(boost::vertex(1, TA), boost::vertex(2, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 0);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 1);

  transition = boost::edge(boost::vertex(2, TA), boost::vertex(0, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 0);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 1);

  transition = boost::edge(boost::vertex(3, TA), boost::vertex(0, TA), TA).first;

  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transition).resetVars.size(), 0);
  BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transition).size(), 1);
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

BOOST_AUTO_TEST_CASE(parseBoostTASimpleTest1)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small3.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  constexpr const std::size_t expected_num_vertices = 3;
  BOOST_CHECK_EQUAL(boost::num_vertices(TA), expected_num_vertices);

  std::array<bool, expected_num_vertices> initResult  = {{true, false, false}};
  std::array<bool, expected_num_vertices> matchResult = {{false, false, true}};
  std::array<std::size_t, expected_num_vertices> labelNumResult  = {{2, 1, 0}};
  std::array<int, expected_num_vertices> labelXResult  = {{0, 0, 0}};
  std::array<int, expected_num_vertices> labelCResult  = {{100, 100, 0}};
  std::array<Constraint<ClockVariables>::Order, expected_num_vertices> labelOdrResult  = {{Constraint<ClockVariables>::Order::gt,
                                                                       Constraint<ClockVariables>::Order::gt, 
                                                                       Constraint<ClockVariables>::Order::gt}};

  for (std::size_t i = 0; i < expected_num_vertices; i++) {
    BOOST_CHECK_EQUAL(TA[i].isInit,  initResult[i]);
    BOOST_CHECK_EQUAL(TA[i].isMatch, matchResult[i]);
    BOOST_CHECK_EQUAL(TA[i].label.size(), labelNumResult[i]);
    if (labelNumResult[i]) {
      BOOST_CHECK_EQUAL(TA[i].label[0].x, labelXResult[i]);
      BOOST_CHECK_EQUAL(TA[i].label[0].c, labelCResult[i]);
      BOOST_TEST((TA[i].label[0].odr == labelOdrResult[i]));
    }
  }

  constexpr const std::size_t expected_num_edges = 3;

  std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::edge_descriptor, expected_num_edges> transitions = {{
      boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first,
      boost::edge(boost::vertex(1, TA), boost::vertex(2, TA), TA).first,
      boost::edge(boost::vertex(2, TA), boost::vertex(0, TA), TA).first,
    }};

  std::array<std::size_t, expected_num_edges> resetVarNumResult  = {{1, 0, 0}};
  std::array<std::size_t, expected_num_edges> guardNumResult  = {{0, 1, 1}};
  std::array<int, expected_num_edges> guardXResult  = {{0, 0, 0}};
  std::array<int, expected_num_edges> guardCResult  = {{0, 4, 5}};
  std::array<Constraint<ClockVariables>::Order, expected_num_edges> guardOdrResult  = {{Constraint<ClockVariables>::Order::lt,
                                                                       Constraint<ClockVariables>::Order::lt, 
                                                                       Constraint<ClockVariables>::Order::gt}};

  for (std::size_t i = 0; i < expected_num_edges; i++) {
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transitions[i]).resetVars.size(), resetVarNumResult[i]);
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i]).size(), guardNumResult[i]);
    if (guardNumResult[i]) {
      BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].x, guardXResult[i]);
      BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].c, guardCResult[i]);
      BOOST_TEST((boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].odr == guardOdrResult[i]));
    }
  }
}

// Purpose: check the parse of orders
BOOST_AUTO_TEST_CASE(parseBoostTASmall4Test)
{
  using SignalVariables = uint8_t;
  using ClockVariables = uint8_t;
  BoostTimedAutomaton<SignalVariables, ClockVariables> TA;
  std::ifstream file("../test/small4.dot");
  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> initStates;
  parseBoostTA(file, TA, initStates);

  constexpr const std::size_t expected_num_vertices = 4;
  BOOST_CHECK_EQUAL(boost::num_vertices(TA), expected_num_vertices);

  std::array<bool, expected_num_vertices> initResult  = {{false, false, false, false}};
  std::array<bool, expected_num_vertices> matchResult = {{false, false, false, false}};
  std::array<std::size_t, expected_num_vertices> labelNumResult  = {{1, 1, 1, 1}};
  std::array<int, expected_num_vertices> labelXResult  = {{0, 0, 0, 0}};
  std::array<int, expected_num_vertices> labelCResult  = {{100, 100, 100, 100}};
  std::array<Constraint<ClockVariables>::Order, expected_num_vertices> labelOdrResult  = {{Constraint<ClockVariables>::Order::gt,
                                                                                           Constraint<ClockVariables>::Order::lt, 
                                                                                           Constraint<ClockVariables>::Order::ge,
                                                                                           Constraint<ClockVariables>::Order::le}};

  for (std::size_t i = 0; i < expected_num_vertices; i++) {
    BOOST_CHECK_EQUAL(TA[i].isInit,  initResult[i]);
    BOOST_CHECK_EQUAL(TA[i].isMatch, matchResult[i]);
    BOOST_CHECK_EQUAL(TA[i].label.size(), labelNumResult[i]);
    if (labelNumResult[i]) {
      BOOST_CHECK_EQUAL(TA[i].label[0].x, labelXResult[i]);
      BOOST_CHECK_EQUAL(TA[i].label[0].c, labelCResult[i]);
      BOOST_TEST((TA[i].label[0].odr == labelOdrResult[i]));
    }
  }

  constexpr const std::size_t expected_num_edges = 4;

  std::array<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::edge_descriptor, expected_num_edges> transitions = {{
      boost::edge(boost::vertex(0, TA), boost::vertex(1, TA), TA).first,
      boost::edge(boost::vertex(0, TA), boost::vertex(2, TA), TA).first,
      boost::edge(boost::vertex(0, TA), boost::vertex(3, TA), TA).first,
      boost::edge(boost::vertex(0, TA), boost::vertex(0, TA), TA).first,
    }};

  std::array<std::size_t, expected_num_edges> resetVarNumResult  = {{0, 0, 0, 0}};
  std::array<std::size_t, expected_num_edges> guardNumResult  = {{1, 1, 1, 1}};
  std::array<int, expected_num_edges> guardXResult  = {{0, 0, 0, 0}};
  std::array<int, expected_num_edges> guardCResult  = {{1, 1, 1, 1}};
  std::array<Constraint<ClockVariables>::Order, expected_num_edges> guardOdrResult  = {{Constraint<ClockVariables>::Order::lt,
                                                                                        Constraint<ClockVariables>::Order::gt, 
                                                                                        Constraint<ClockVariables>::Order::ge,
                                                                                        Constraint<ClockVariables>::Order::le}};

  for (std::size_t i = 0; i < expected_num_edges; i++) {
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::resetVars, TA, transitions[i]).resetVars.size(), resetVarNumResult[i]);
    BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i]).size(), guardNumResult[i]);
    if (guardNumResult[i]) {
      BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].x, guardXResult[i]);
      BOOST_CHECK_EQUAL(boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].c, guardCResult[i]);
      BOOST_TEST((boost::get(&BoostTATransition<ClockVariables>::guard, TA, transitions[i])[0].odr == guardOdrResult[i]));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
