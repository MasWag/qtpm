#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>
#include <type_traits>

#include "dbm.hh"
#include "constraint.hh"
#include "timed_automaton.hh"


template<class SignalVariables, class ClockVariables>
struct BoostZoneGraphState {
  typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor vertex;
  DBM zone;
};

template<class SignalVariables, class ClockVariables, class Weight>
struct BoostZoneGraphTransition {
  //! @note this structure is necessary because of some problem in boost graph
  Weight weight;
  typename BoostTimedAutomaton<SignalVariables, ClockVariables>::edge_descriptor edge;
};

template<class SignalVariables, class ClockVariables, class Weight>
using BoostZoneGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, BoostZoneGraphState<SignalVariables, ClockVariables>, BoostZoneGraphTransition<SignalVariables, ClockVariables, Weight>>;

template<class SignalVariables, class ClockVariables, class Weight>
void zoneConstruction(const BoostTimedAutomaton<SignalVariables, ClockVariables> &TA,
                      const std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> &initStatesTA,
                      BoostZoneGraph<SignalVariables, ClockVariables, Weight> &ZG,
                      std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> &initStatesZG) {
  using TA_t = BoostTimedAutomaton<SignalVariables, ClockVariables>;
  using ZG_t = BoostZoneGraph<SignalVariables, ClockVariables, Weight>;
  boost::unordered_map<std::pair<typename TA_t::vertex_descriptor, DBM::Tuple>, typename ZG_t::vertex_descriptor> toZGState;
  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  auto zeroDBM = DBM::zero(num_of_vars + 1);
  zeroDBM.M = max_constraints;

  initStatesZG.reserve(initStatesTA.size());
  for (const auto &initState: initStatesTA) {
    auto v = boost::add_vertex(ZG);
    ZG[v].vertex = initState;
    ZG[v].zone = zeroDBM;
    initStatesZG.push_back(v);

    toZGState[std::make_pair(initState, zeroDBM.toTuple())] = v;
  }
  auto nextConf = initStatesZG;

  while (!nextConf.empty()) {
    auto currentConf = std::move(nextConf);
    nextConf.clear();

    for (auto &conf : currentConf) {
      auto taState = ZG[conf].vertex;
      DBM nowZone = ZG[conf].zone;

      nowZone.elapse();
      for (auto range = boost::out_edges(taState, TA); range.first != range.second; range.first++) {
        const auto edge = *range.first;
        DBM nextZone = nowZone;
        auto nextState = boost::target(edge, TA);

        const auto guard = TA[edge].guard;
        for (const auto &delta : guard) {
          switch (delta.odr) {
          case Constraint<ClockVariables>::Order::lt:
          case Constraint<ClockVariables>::Order::le:
            nextZone.tighten(delta.x,-1, delta.c);
            break;
          case Constraint<ClockVariables>::Order::gt:
          case Constraint<ClockVariables>::Order::ge:
            nextZone.tighten(-1,delta.x, -delta.c);
            break;
          }
        }

        if (nextZone.isSatisfiable()) {
          for (auto x : TA[edge].resetVars.resetVars) {
            nextZone.reset(x);
          }
          nextZone.abstractize();
          nextZone.canonize();

          auto zgState = toZGState.find(std::make_pair(nextState, nextZone.toTuple()));

          if (zgState != toZGState.end()) {
            // targetStateInZA is already added
            auto zgEdge = std::get<0>(boost::add_edge(conf, zgState->second, ZG));
            ZG[zgEdge].edge = edge;

          } else {
            // targetStateInZA is new
            auto zgState = boost::add_vertex(ZG);
            ZG[zgState].vertex = nextState;
            ZG[zgState].zone = nextZone;
            toZGState[std::make_pair(nextState, nextZone.toTuple())] = zgState;
            auto zgEdge = std::get<0>(boost::add_edge(conf, zgState, ZG));
            ZG[zgEdge].edge = edge;

            nextConf.push_back (zgState);
          }
        }
      }
    }
  }
}
