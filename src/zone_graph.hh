#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>
#include <type_traits>

#include "dbm.hh"
#include "constraint.hh"
#include "timed_automaton.hh"


template<class SignalVariables, class ClockVariables, class Value>
struct BoostZoneGraphState {
  typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor vertex;
  bool jumpable;
  DBM zone;
  std::vector<std::vector<Value>> valuations;
};

template<class SignalVariables, class ClockVariables, class Weight, class Value>
using BoostZoneGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, BoostZoneGraphState<SignalVariables, ClockVariables, Value>, boost::property<boost::edge_weight_t, Weight>>;

template<class SignalVariables, class ClockVariables, class Weight, class Value>
void zoneConstruction(const BoostTimedAutomaton<SignalVariables, ClockVariables> &TA,
                      const std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> &initStatesTA,
                      BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> &ZG,
                      std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> &initStatesZG) {
  using TA_t = BoostTimedAutomaton<SignalVariables, ClockVariables>;
  using ZG_t = BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>;
  boost::unordered_map<std::pair<typename TA_t::vertex_descriptor, DBM::Tuple>, typename ZG_t::vertex_descriptor> toZGState;
  const auto max_constraints = boost::get_property(TA, boost::graph_max_constraints);
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
  auto zeroDBM = DBM::zero(num_of_vars + 1);
  zeroDBM.M = Bounds{max_constraints, true};

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

    for (auto &currentZGState : currentConf) {
      auto taState = ZG[currentZGState].vertex;
      DBM nowZone = ZG[currentZGState].zone;

      nowZone.elapse();
      for (auto range = boost::out_edges(taState, TA); range.first != range.second; range.first++) {
        const auto edge = *range.first;
        DBM nextZone = nowZone;
        auto nextState = boost::target(edge, TA);

        const auto guard = TA[edge].guard;
        for (const auto &delta : guard) {
          switch (delta.odr) {
          case Constraint<ClockVariables>::Order::lt:
            nextZone.tighten(delta.x,-1,{delta.c, false});
          case Constraint<ClockVariables>::Order::le:
            nextZone.tighten(delta.x,-1,{delta.c, true});
            break;
          case Constraint<ClockVariables>::Order::gt:
            nextZone.tighten(-1,delta.x,{-delta.c, false});
          case Constraint<ClockVariables>::Order::ge:
            nextZone.tighten(-1,delta.x,{-delta.c, true});
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
            boost::add_edge(currentZGState, zgState->second, ZG);

          } else {
            // targetStateInZA is new
            auto zgState = boost::add_vertex(ZG);
            ZG[zgState].vertex = nextState;
            ZG[zgState].zone = nextZone;
            toZGState[std::make_pair(nextState, nextZone.toTuple())] = zgState;
            boost::add_edge(currentZGState, zgState, ZG);

            nextConf.push_back (zgState);
          }
        }
      }
    }
  }
}


/*!
  @brief Zone construction with an additional clock variable.
  @tparam SignalVariables 
  @tparam ClockVariables
  @tparam CostFunction
  @tparam Weight
  @param [in] TA A timed automaton.
  @param [in] initConfTA Initial configuarion of the timed automaton
  @param [in] cost A cost function.
  @param [in] valuation A data valuation
  @param [in] duartion A length of the signal
  @param [out] ZG The zone graph with weight.
  @param [out] initStatesZG The initial states of the zone graph.
*/
template<class SignalVariables, class ClockVariables, class Weight, class Value>
void zoneConstructionWithT(const BoostTimedAutomaton<SignalVariables, ClockVariables> &TA,
                           const std::vector<std::pair<BoostZoneGraphState<SignalVariables, ClockVariables, Value>, Weight>> &initConfTA,
                           const std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> &cost,
                           const std::vector<Value> &valuation,
                           const double duration,
                           BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value> &ZG,
                           std::unordered_map<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor,Weight> &initStatesZG) {
  using TA_t = BoostTimedAutomaton<SignalVariables, ClockVariables>;
  using ZG_t = BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>;
  using TAState = typename TA_t::vertex_descriptor;
  boost::unordered_map<std::tuple<typename TA_t::vertex_descriptor, bool, DBM::Tuple, std::vector<std::vector<Value>>>, typename ZG_t::vertex_descriptor> toZGState;
  // const double max_constraints = std::max<double>(ceil(duration), boost::get_property(TA, boost::graph_max_constraints));
#ifdef DEBUG
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);
#endif

  const auto convToKey = [] (BoostZoneGraphState<SignalVariables, ClockVariables, Value> x) {
                           return std::make_tuple(x.vertex, x.jumpable, x.zone.toTuple(), x.valuations);
                         };
  const auto dwellTimeClockVar = initConfTA.front().first.zone.getNumOfVar() - 1;

  std::vector<typename BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor> nextConf;
  nextConf.reserve(initConfTA.size());
  for (const auto &initState: initConfTA) {
    auto v = boost::add_vertex(ZG);
    ZG[v] = initState.first;
    // the zone must contain the new clock variable T for the dwell time.
    // we admit > ... + 1 to use this function for timed pattern matching too.
#ifdef DEBUG
    assert(ZG[v].zone.getNumOfVar() >= num_of_vars + 1);
#endif
    ZG[v].zone.tighten(dwellTimeClockVar, -1, {duration, true});

    initStatesZG[v] = initState.second;
    nextConf.push_back(v);

    toZGState[convToKey(ZG[v])] = v;
  }

  const auto addEdge = [&toZGState,&ZG,&nextConf,&TA,&cost,&convToKey] (const auto currentZGState, const auto nextTAState, const bool jumpable, const DBM &zone, const std::vector<std::vector<Value>> &nextValuations) -> bool {
                         auto zgState = toZGState.find(std::make_tuple(nextTAState, jumpable, zone.toTuple(), nextValuations));
                         typename ZG_t::edge_descriptor edge;

                         const bool isNew = zgState == toZGState.end();

                         if (!isNew) {
                           // targetStateInZA is already added
                           // TODO: we can merge some edges
                           edge = std::get<0>(boost::add_edge(currentZGState, zgState->second, ZG));
                         } else {
                           // targetStateInZA is new
                           auto nextZGState = boost::add_vertex(ZG);
                           ZG[nextZGState].vertex = nextTAState;
                           ZG[nextZGState].jumpable = jumpable;
                           ZG[nextZGState].zone = zone;
                           ZG[nextZGState].valuations = nextValuations;
                           toZGState[convToKey(ZG[nextZGState])] = nextZGState;
                           edge = std::get<0>(boost::add_edge(currentZGState, nextZGState, ZG));
                           if (!jumpable) {
                             nextConf.push_back (nextZGState);
                           }
#ifdef DEBUG
                           assert((toZGState.find(convToKey(ZG[nextZGState])) != toZGState.end()));
#endif
                         }

                         if (!jumpable) {
                           boost::put(boost::edge_weight, ZG, edge, cost(TA[ZG[currentZGState].vertex].label,
                                                                         ZG[currentZGState].valuations));
                         } else {
                           boost::put(boost::edge_weight, ZG, edge, Weight::one());
                         }

                         return isNew;
                       };


  while (!nextConf.empty()) {
    auto currentConf = std::move(nextConf);
    nextConf.clear();

    for (const auto &currentZGState : currentConf) {
      auto taState = ZG[currentZGState].vertex;
      bool jumpable = ZG[currentZGState].jumpable;
      DBM nowZone = ZG[currentZGState].zone;

      if (nowZone.value.cols() == 0) {
        // when the vertex does not exist
        continue;
      }

      nowZone.tighten(dwellTimeClockVar, -1, {duration, true});

      const auto listDiscreteTransitions =
        [&TA,&taState] (const DBM& nowZone, std::vector<std::pair<TAState, DBM>> &v) {
          // discrete transition
          for (auto range = boost::out_edges(taState, TA); range.first != range.second; range.first++) {
            const auto edge = *range.first;
            DBM nextZone = nowZone;
            auto nextTAState = boost::target(edge, TA);

            const auto guard = TA[edge].guard;
            for (const auto &delta : guard) {
              switch (delta.odr) {
              case Constraint<ClockVariables>::Order::lt:
                nextZone.tightenWithoutClose(delta.x,-1,{delta.c, false});
              case Constraint<ClockVariables>::Order::le:
                nextZone.tightenWithoutClose(delta.x,-1,{delta.c, true});
                break;
              case Constraint<ClockVariables>::Order::gt:
                nextZone.tightenWithoutClose(-1,delta.x,{-delta.c, false});
              case Constraint<ClockVariables>::Order::ge:
                nextZone.tightenWithoutClose(-1,delta.x,{-delta.c, true});
              }
            }

            if (nextZone.isSatisfiable()) {
              for (auto x : TA[edge].resetVars.resetVars) {
                nextZone.reset(x);
              }

              v.emplace_back(nextTAState, nextZone);
            }
          }
        };

      if (jumpable) {
        // discrete transition
        std::vector<std::pair<TAState, DBM>> nextTAStates;
        listDiscreteTransitions(nowZone, nextTAStates);

        if (nextTAStates.empty()) {
          // If there is no out going transition, it checks if there is a transition later.
          auto futureZone = nowZone;
          futureZone.elapse();
          std::vector<std::pair<TAState, DBM>> futureTAStates;
          listDiscreteTransitions(nowZone, futureTAStates);
          // If there is no transition in the future, the current state is useless and we remove it.
          if (futureTAStates.empty()) {
            const auto nextZGStateP = toZGState.find(convToKey(ZG[currentZGState]));
            if (nextZGStateP != toZGState.end()) {
              clear_vertex(nextZGStateP->second, ZG);
              remove_vertex(nextZGStateP->second, ZG);
              toZGState.erase(nextZGStateP);
            }
          }
          continue;
        }
        for (auto &p: nextTAStates) {
          addEdge(currentZGState, std::move(p.first), false, std::move(p.second), {});
        }

      } else {
        // continuous transition
        auto nextValuations = ZG[currentZGState].valuations;

        nextValuations.push_back(valuation);
        nowZone.elapse();
        nowZone.tighten(dwellTimeClockVar, -1, {duration, true});
        if (!nowZone.isSatisfiableWithoutCanonize()) {
          continue;
        }

        std::vector<std::pair<TAState, DBM>> nextTAStates;
        listDiscreteTransitions(nowZone, nextTAStates);
        // We add the state only if it has a next state

        if (!nextTAStates.empty()) {
          bool isNew = addEdge(currentZGState, ZG[currentZGState].vertex, true, nowZone, nextValuations);
          if (isNew) {
            const auto nextZGStateP = toZGState.find(std::make_tuple(ZG[currentZGState].vertex, true, nowZone.toTuple(), nextValuations));
            for (auto &p: nextTAStates) {
              addEdge(nextZGStateP->second, std::move(p.first), false, std::move(p.second), {});
            }
          }
        } else {
          // if the state is accepting, the state is useful even if it has no outgoing transition.
          if (TA[ZG[currentZGState].vertex].isMatch) {
            continue;
          }            
          // If there is no out going transition, it checks if there is a transition later.
          auto futureZone = nowZone;
          futureZone.elapse();
          std::vector<std::pair<TAState, DBM>> futureTAStates;
          listDiscreteTransitions(nowZone, futureTAStates);
          // If there is no transition in the future, the current state is useless and we remove it.
          if (futureTAStates.empty()) {
            const auto nextZGStateP = toZGState.find(convToKey(ZG[currentZGState]));
            // If we cannot go out and the state already exists, we remove the state.
            if (nextZGStateP != toZGState.end()) {
              clear_vertex(nextZGStateP->second, ZG);
              remove_vertex(nextZGStateP->second, ZG);
              toZGState.erase(nextZGStateP);
            }
          }
        }

        // nowZone.canonize();
      }
    }
  }
}
