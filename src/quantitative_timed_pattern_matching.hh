#pragma once
#include <vector>
#include <queue>

#include "bellman_ford.hh"
#include "zone_graph.hh"

/*!
  @brief A class to execute quantitative timed pattern matching

  @note This class works in an incremental way.

  @section Notes on the Usage of DBM

  The usage of each cell in the DBM is as follows.

  - 0: x0 == 0
  - 1-N: x (usual variables)
  - N+1: the duration from the actual start
      - i.e., NOW - t
  - N+2: the dwell time
      - i.e., $\mathit{NOW} - \tau_i$
      - should be released at first
      - THIS SHOULD NOT RESET in zone construction

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

  using ConfTuple_t = std::tuple<TAState, bool, std::vector<std::vector<Value>>, Weight>;

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
  boost::unordered_map<ResultMatrix, Weight> result = {};

public:

  QuantitativeTimedPatternMatching(const TimedAutomaton &TA,
                                   const std::vector<TAState> &initStates,
                                   const std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> &cost) : numOfClockVariables(boost::get_property(TA, boost::graph_num_of_vars)), TA(TA), initStates(initStates), cost(cost) {
    DBM z = DBM::zero(numOfClockVariables + 1 + 2);
    // release Z(N+2)
    z.M = Bounds(std::numeric_limits<double>::infinity(), false);
    z.release(numOfClockVariables + 2 - 1);
    z.tightenWithoutClose(-1, numOfClockVariables + 2 - 1, {0, true});
    z.canonize();
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
      // reset Z(N+2)
      c.first.zone.reset(numOfClockVariables + 2 - 1);
      if (c.first.jumpable) {
        c.first.valuations.push_back(valuation);
        c.first.zone.elapse();
      }
    }

    configuration.reserve(configuration.size() + initStates.size());

    for(const auto &q0: initStates) {
      configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q0, false, initialZone, std::vector<std::vector<Value>>{}}, Weight::one());
    }

    ZoneGraph ZG;
    std::unordered_map<ZGState, Weight> initStatesZG;
    zoneConstructionWithT(TA, configuration, cost, valuation, duration, ZG, initStatesZG);
#ifdef DEBUG
    assert(std::none_of(initStatesZG.begin(), initStatesZG.end(), [&](auto p) {
          return TA[ZG[p.first].vertex].isMatch;
        }));
#endif

    std::unordered_map<ZGState, Weight> distance;
    bellman_ford<std::queue<ZGState>>(ZG, initStatesZG, distance);
    // This does not work when VerticesList is ListS. I do not know why.
    // write_graphviz(std::cerr, ZG, makeZoneGraphLabelWriter(ZG, TA, distance),make_weight_label_writer(ZG));
    configuration.clear();

    boost::unordered_map<ConfTuple_t, std::list<DBM>> confMap;

    for (auto w: distance) {
      if (!TA[ZG[w.first].vertex].isMatch && ZG[w.first].zone.value.cols() > 0) {
        auto z = ZG[w.first].zone;
        z.tightenWithoutClose(-1, numOfClockVariables + 2 - 1, Bounds{-duration, true});
        z.tightenWithoutClose(numOfClockVariables + 2 - 1, -1, Bounds{duration, true});
        if (z.isSatisfiable()) {

          auto it = confMap.find(std::make_tuple(ZG[w.first].vertex, ZG[w.first].jumpable, ZG[w.first].valuations ,w.second));
          if (it != confMap.end()) {
          for (DBM &zz: it->second) {
            if (zz.merge(z)) {
              goto next;
            }
          }
          it->second.push_back(std::move(z));
          } else {
            confMap[std::make_tuple(ZG[w.first].vertex, ZG[w.first].jumpable, ZG[w.first].valuations ,w.second)].push_back(std::move(z));
          }
        }
      }
    next:;
    }

    for (auto &c: confMap) {
      if (c.second.size() == 1) {
        configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{std::get<0>(c.first), std::get<1>(c.first), c.second.front(), std::get<2>(c.first)}, std::get<3>(c.first));

      } else {
        bool removed = true;
        while (removed) {
          removed = false;
          for (auto it = c.second.begin(); it != c.second.end(); it++) {
            DBM tmp = *it;
            auto jt = it;
            for (jt++; jt != c.second.end();) {
              if (it->merge(*jt)) {
                jt = c.second.erase(jt);
                removed = true;
              } else {
                jt++;
              }
            }
          }
        }
        for (auto &z: c.second) {
          // for the current check
          configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{std::get<0>(c.first), std::get<1>(c.first), std::move(z), std::get<2>(c.first)}, std::get<3>(c.first));
        }
      }
    }

    for (auto &w: distance) {
      if (TA[ZG[w.first].vertex].isMatch && !ZG[w.first].jumpable && ZG[w.first].zone.value.cols() > 0) {
        //        assert(ZG[w.first].zone.isSatisfiable());
        ResultMatrix mat = {{ZG[w.first].zone.value(numOfClockVariables + 2 - 1, numOfClockVariables + 2) - absTime,
                             ZG[w.first].zone.value(numOfClockVariables + 2, numOfClockVariables + 2 - 1) + absTime,
                             ZG[w.first].zone.value(0, numOfClockVariables + 2) - absTime,
                             ZG[w.first].zone.value(numOfClockVariables + 2, 0) + absTime,
                             ZG[w.first].zone.value(0, numOfClockVariables + 2 - 1),
                             ZG[w.first].zone.value(numOfClockVariables + 2 - 1, 0)}};

        if (result.find(mat) == result.end()) {
          result[std::move(mat)] = std::move(w.second);
        } else {
          result[std::move(mat)] += std::move(w.second);
        }
      }
    }

    absTime += duration;
  }

  void getResult(boost::unordered_map<ResultMatrix, Weight> &v) const {
    v = result;
  }
  boost::unordered_map<ResultMatrix, Weight>& getResultRef()  {
    return result;
  }
};
