#pragma once
#include <vector>
#include <queue>

#include "bellman_ford.hh"
#include "zone_graph.hh"

/*!
  @brief A class to execute quantitative timed pattern matching

  @note This class works in an incremental way.
 */
template<class SignalVariables, class ClockVariables, class Weight, class Value>
class QuantitativeTimedPatternMatching
{
public:
  using ResultMatrix = std::array<Bounds, 6>;
private:
  //types

  using Conf_t = std::vector<std::pair<BoostZoneGraphState<SignalVariables, ClockVariables, Value>, Weight>>;
  using TimedAutomaton = BoostTimedAutomaton<SignalVariables, ClockVariables>;
  using TAState = typename TimedAutomaton::vertex_descriptor;
  using ZoneGraph = BoostZoneGraph<SignalVariables, ClockVariables, Weight, Value>;
  using ZGState = typename ZoneGraph::vertex_descriptor;

  // constants

  const std::size_t numOfClockVariables;
  DBM initialZone;
  const TimedAutomaton TA;
  const std::vector<TAState> initStates;
  const std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> cost;

  // variables

  //! @brief the current configuration
  Conf_t configuration = {};
  //! @brief the current absolute time
  double absTime = 0;
  /*! 
    @brief result vector
    @todo consider better data structure (something like segment tree)
  */
  std::vector<std::pair<ResultMatrix, Weight>> result = {};

public:

  QuantitativeTimedPatternMatching(const TimedAutomaton &TA,
                                   const std::vector<TAState> &initStates,
                                   const std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> &cost) : numOfClockVariables(boost::get_property(TA, boost::graph_num_of_vars)), TA(TA), initStates(initStates), cost(cost) {
    DBM z = DBM::zero(numOfClockVariables + 1 + 2);
    z.release(numOfClockVariables + 2 - 1);
    initialZone = std::move(z);
  }
  /*!
    @brief feed one valuation with dwell time
    @param [in] valuation The new signal valuation
    @param [in] duration The duration of the given signal valuation
    @note It is not a problem to give the same valuation consecutively.
   */
  void feed(const std::vector<Value> &valuation, const double duration) {
    for (auto &c: configuration) {
      c.first.zone.reset(numOfClockVariables + 2 - 1);
      if (c.first.jumpable) {
        c.first.valuations.push_back(valuation);
      }
    }

    configuration.reserve(configuration.size() + initStates.size());

    for(const auto &q0: initStates) {
      configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q0, false, initialZone, std::vector<std::vector<Value>>{}}, Weight::one());
    }

    ZoneGraph ZG;
    std::unordered_map<ZGState, Weight> initStatesZG;
    zoneConstructionWithT(TA, configuration, cost, valuation, duration, ZG, initStatesZG);

    std::unordered_map<ZGState, Weight> distance;
    bellman_ford<std::queue<ZGState>>(ZG, initStatesZG, distance);
    configuration.clear();

    for (auto w: distance) {
      auto z = ZG[w.first].zone;
      z.tighten(-1, numOfClockVariables + 2 - 1, Bounds{-duration, true});
      z.tighten(numOfClockVariables + 2 - 1, -1, Bounds{duration, true});
      if (z.isSatisfiable()) {
        configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{ZG[w.first].vertex, ZG[w.first].jumpable, z, ZG[w.first].valuations}, w.second);
      }
    }

    for (auto &w: distance) {
      if (TA[ZG[w.first].vertex].isMatch) {
        //        assert(ZG[w.first].zone.isSatisfiable());
        ResultMatrix mat = {{ZG[w.first].zone.value(numOfClockVariables + 2 - 1, numOfClockVariables + 2) - absTime,
                             ZG[w.first].zone.value(numOfClockVariables + 2, numOfClockVariables + 2 - 1) + absTime,
                             ZG[w.first].zone.value(0, numOfClockVariables + 2) - absTime,
                             ZG[w.first].zone.value(numOfClockVariables + 2, 0) + absTime,
                             ZG[w.first].zone.value(0, numOfClockVariables + 2 - 1),
                             ZG[w.first].zone.value(numOfClockVariables + 2 - 1, 0)}};

        result.emplace_back(std::move(mat), std::move(w.second));
      }
    }

    absTime += duration;
  }

  void getResult(std::vector<std::pair<ResultMatrix, Weight>> &v) const {
    v = result;
  }
  std::vector<std::pair<ResultMatrix, Weight>>& getResultRef()  {
    return result;
  }
};