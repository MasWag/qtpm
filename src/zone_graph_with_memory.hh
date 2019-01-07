#pragma once
#include "zone_graph.hh"

template<class SignalVariables, class ClockVariables, class Value>
struct WZTSMState {
  typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor vertex;
  DBM zone;
  std::vector<double> memoryValuations;
  std::vector<std::vector<Value>> valuations;
};

/*!
  @brief Weightez zone transition system with memory
 */
template<class SignalVariables, class ClockVariables, class Weight, class Value>
using WZTSM = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, WZTSMState<SignalVariables, ClockVariables, Value>, boost::property<boost::edge_weight_t, Weight>>;

/*!
  @brief Zone construction with an additional clock variable.
  @tparam SignalVariables 
  @tparam ClockVariables
  @tparam MemoryVariables
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
template<class SignalVariables, class ClockVariables, class MemoryVariables, class Weight, class Value>
void zoneConstructionWithT(const TSAM<SignalVariables, ClockVariables, MemoryVariables> &TA,
                           const std::vector<std::pair<WZTSMState<SignalVariables, ClockVariables, Value>, Weight>> &initConfTA,
                           const std::function<Weight(const std::shared_ptr<ComplicatedConstraint>&,
                                                      const std::vector<double>&,
                                                      const std::vector<std::vector<Value>> &)> &cost,
                           const std::vector<Value> &valuation,
                           const Bounds &beginBound, 
                           const Bounds &endBound,
                           WZTSM<SignalVariables, ClockVariables, Weight, Value> &ZG,
                           std::unordered_map<typename WZTSM<SignalVariables, ClockVariables, Weight, Value>::vertex_descriptor,Weight> &initStatesZG) {
  using TA_t = TSAM<SignalVariables, ClockVariables, MemoryVariables>;
  using ZG_t = WZTSM<SignalVariables, ClockVariables, Weight, Value>;
  boost::unordered_map<std::tuple<typename TA_t::vertex_descriptor, bool, DBM::Tuple, std::vector<double>, std::vector<std::vector<Value>>>, typename ZG_t::vertex_descriptor> toZGState;
  // const double max_constraints = std::max<double>(ceil(duration), boost::get_property(TA, boost::graph_max_constraints));
  const auto num_of_vars = boost::get_property(TA, boost::graph_num_of_vars);

  const auto convToKey = [] (WZTSMState<SignalVariables, ClockVariables, Value> x) {
                           return std::make_tuple(x.vertex, x.jumpable, x.zone.toTuple(), x.memoryValuations, x.valuations);
                         };
  const auto dwellTimeClockVar = initConfTA.front().first.zone.getNumOfVar() - 1;

  std::vector<typename ZG_t::vertex_descriptor> nextConf;
  nextConf.reserve(initConfTA.size());
  for (const auto &initState: initConfTA) {
    auto v = boost::add_vertex(ZG);
    ZG[v].vertex = initState.first.vertex;
    ZG[v].memoryValuations = initState.first.memoryValuations;
    ZG[v].zone = initState.first.zone;
    ZG[v].valuations = initState.first.valuations;
    // the zone must contain the new clock variable T for the dwell time.
    // we admit > ... + 1 to use this function for timed pattern matching too.
    assert(ZG[v].zone.getNumOfVar() >= num_of_vars + 1);

    initStatesZG[v] = initState.second;
    nextConf.push_back(v);

    toZGState[convToKey(initState.first)] = v;
  }

  const auto addEdge = [&toZGState,&ZG,&nextConf,&TA,&cost] (const auto currentZGState, const auto nextTAState, const DBM &zone, const std::vector<double>& memoryValuations, const std::vector<std::vector<Value>> &nextValuations) {
                         auto zgState = toZGState.find(std::make_tuple(nextTAState, zone.toTuple(), memoryValuations, nextValuations));
    typename ZG_t::edge_descriptor edge;


    if (zgState != toZGState.end()) {
      // targetStateInZA is already added
      // TODO: we can merge some edges
      edge = std::get<0>(boost::add_edge(currentZGState, zgState->second, ZG));

    } else {
      // targetStateInZA is new
      auto nextZGState = boost::add_vertex(ZG);
      ZG[nextZGState].vertex = nextTAState;
      ZG[nextZGState].memoryValuations = memoryValuations;
      ZG[nextZGState].zone = zone;
      ZG[nextZGState].valuations = nextValuations;
      toZGState[std::make_tuple(nextTAState, zone.toTuple(), memoryValuations, nextValuations)] = nextZGState;
      edge = std::get<0>(boost::add_edge(currentZGState, nextZGState, ZG));

      nextConf.push_back (nextZGState);
    }

    if (nextValuations.empty()) {
      boost::put(boost::edge_weight, ZG, edge, cost(TA[ZG[currentZGState].vertex].label,
                                                    ZG[currentZGState].memoryValuations,
                                                    ZG[currentZGState].valuations));
    } else {
      boost::put(boost::edge_weight, ZG, edge, Weight::one());
    }
  };


  while (!nextConf.empty()) {
    auto currentConf = std::move(nextConf);
    nextConf.clear();

    for (const auto &currentZGState : currentConf) {
      auto taState = ZG[currentZGState].vertex;
      const bool jumpable = !ZG[currentZGState].valuations.empty();
      DBM nowZone = ZG[currentZGState].zone;

      if (jumpable) {
        // discrete transition
        for (auto range = boost::out_edges(taState, TA); range.first != range.second; range.first++) {
          const auto edge = *range.first;
          DBM nextZone = nowZone;
          auto nextTAState = boost::target(edge, TA);

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
            }
          }

          if (nextZone.isSatisfiable()) {
            for (auto x : TA[edge].resetVars.resetVars) {
              nextZone.reset(x);
            }
            nextZone.canonize();

            addEdge(currentZGState, nextTAState, false, nextZone, {});
          }
        }        
      } else {
        // continuous transition
        auto nextValuations = ZG[currentZGState].valuations;
        if (nextValuations.empty() || nextValuations.back() == valuation) {
          nextValuations.push_back(valuation);
        }
        nowZone.elapse();
        nowZone.tighten(-1, dwellTimeClockVar, beginBound);
        nowZone.tighten(dwellTimeClockVar, -1, endBound);
        nowZone.canonize();
        addEdge(currentZGState, ZG[currentZGState].vertex, true, nowZone, nextValuations);
      }
    }
  }
}
