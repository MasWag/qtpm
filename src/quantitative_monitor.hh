#pragma once
#include <vector>
#include <queue>

#include "bellman_ford.hh"
#include "zone_graph_with_memory.hh"

/*!
  @brief A class to execute quantitative monitoring

  @note This class works in an incremental way.

  @section Notes on the Usage of DBM

  The usage of each cell in the DBM is as follows.

  - 0: x0 == 0
  - 1-N: x (usual variables)
  - N+1: the absolute time
  - i.e., $\mathit{NOW}$
  - should be released at first
  - THIS SHOULD NOT RESET in zone construction

*/
template<class SignalVariables, class ClockVariables, class MemoryVariables, class Weight, class Value>
class QuantitativeMonitor
{
public:
private:
  //types
  using TimedAutomaton = TSAM<SignalVariables, ClockVariables, MemoryVariables>;
  using TAState = typename TimedAutomaton::vertex_descriptor;
  using ZoneGraph = WZTSM<SignalVariables, ClockVariables, Weight, Value>;
  using ZGState = typename ZoneGraph::vertex_descriptor;
  using Conf_t = std::vector<std::pair<WZTSMState<SignalVariables, ClockVariables, Value>, Weight>>;

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

public:

  QuantitativeMonitor(const TimedAutomaton &TA,
                      const std::vector<TAState> &initStates,
                      const std::function<Weight(const std::vector<Constraint<ClockVariables>>&,
                                                 const std::vector<std::vector<Value>> &)> &cost) : 
    numOfClockVariables(boost::get_property(TA, boost::graph_num_of_vars)), TA(TA), initStates(initStates), cost(cost) {
    DBM z = DBM::zero(numOfClockVariables + 1 + 1);
    // release Z(N+2)
    z.M = Bounds(std::numeric_limits<double>::infinity(), false);
    z.release(numOfClockVariables + 1 - 1);
    z.tighten(-1, numOfClockVariables + 1 - 1, {0, true});
    initialZone = std::move(z);
  }
  /*!
    @brief feed one valuation with dwell time
    @param [in] valuation The new signal valuation
    @param [in] duration The duration of the given signal valuation
    @note It is not a problem to give the same valuation consecutively.
  */
  void feed(const std::vector<Value> &valuation, const double duration) {

    {
      // ### (absTime, absTime + duration) ###
      Bounds beginBound = Bounds{-absTime, false};
      Bounds endBound = Bounds{absTime + duration, false};
      for (auto &c: configuration) {
        // release Z(N+1) and let Z(T) = (absTime, absTime + duration)
        c.first.zone.release(numOfClockVariables + 1 - 1);
        c.first.zone.elapse();
        c.first.zone.tighten(-1, numOfClockVariables + 2 - 1, beginBound);
        c.first.zone.tighten(numOfClockVariables + 2 - 1, -1, endBound);
        c.first.valuations.push_back(valuation);
      }

      ZoneGraph ZG;
      std::unordered_map<ZGState, Weight> initStatesZG;
      zoneConstructionWithT(TA, configuration, cost, valuation, beginBound, endBound, ZG, initStatesZG);

      std::unordered_map<ZGState, Weight> distance;
      bellman_ford<std::queue<ZGState>>(ZG, initStatesZG, distance);
      configuration.clear();

      for (auto w: distance) {
        configuration.emplace_back(ZG[w.first], w.second);
      }
    }

    absTime += duration;

    {
      // ### [absTime + duration, absTime + duration] ###
      Bounds beginBound = Bounds{-absTime, false};
      Bounds endBound = Bounds{absTime + duration, false};
      for (auto &c: configuration) {
        // release Z(N+1) and let Z(T) = [absTime + duration, absTime + duration]
        c.first.zone.release(numOfClockVariables + 1 - 1);
        c.first.zone.elapse();
        c.first.zone.tighten(-1, numOfClockVariables + 2 - 1, beginBound);
        c.first.zone.tighten(numOfClockVariables + 2 - 1, -1, endBound);
      }

      ZoneGraph ZG;
      std::unordered_map<ZGState, Weight> initStatesZG;
      zoneConstructionWithT(TA, configuration, cost, valuation, duration, ZG, initStatesZG);

      std::unordered_map<ZGState, Weight> distance;
      bellman_ford<std::queue<ZGState>>(ZG, initStatesZG, distance);
      configuration.clear();

      for (auto w: distance) {
        configuration.emplace_back(ZG[w.first], w.second);
      }
    }
  }

  void getResult(Weight &v) const {
    v = Weight::zero();
    for (const auto& conf: configuration) {
      if (TA[conf.first.vertex].isMatch && conf.first.valuations.empty()) {
        v += conf.second;
      }
    }
  }
  Weight getResult() const {
    Weight v;
    getResult(v);
    return std::move(v);
  }
};
