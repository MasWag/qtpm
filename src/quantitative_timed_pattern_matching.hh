#pragma once
#include <vector>
#include <queue>

#include "bellman_ford.hh"
#include "zone_graph.hh"

/*!
  @brief A class to execute quantitative timed pattern matching

  @note This class works in an incremental way.

  @section outline-qtpm Outline of the Automata-Based Algorithm for Quantitative Timed Pattern Matching
  
  The following shows the outline of the algorithm for quantitative timed pattern matching:

  1. A piece @f$(u_i, \tau_i)@f$ of the monitored piecewise constant signal is given to @ref feed.
  2. The configurations corresponding to the matching begging from the current piece are added to the "pool" of the current configurations.
  3. The zone graph of duration @f$\tau_i@f$ with values @f$u_i@f$ is constructed.
  4. For each node of the zone graph, we compute the shortest distance to it by the generalized Bellman-Ford algorithm @cite Mohri09.
  5. By forcing the dwell time, we construct the configuration just after the current piece.
  6. For the configurations reaching accepting states, we put the resulting matching to this->result.

  We use the zone graph for generalized reachability analysis since the transition and the switching of the signal values are asynchronous.

  @section on-the-usage-of-dbm On the Usage of DBM in Quantitative Timed Pattern Matching

  The usage of each element in the DBM is as follows.

  - 0: x0 == 0
      - i.e., the special element showing the constant 0.
  - 1-N: x (usual variables)
  - N+1: the duration from the actual start
      - i.e., @f$ \mathrm{NOW} - t @f$, where @f$ t @f$ is the beginning of the matching
  - N+2: the dwell time
      - i.e., @f$ \mathrm{NOW} - \tau_i @f$
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
  const std::size_t dwellTimeClock;
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
                                   const std::function<Weight(const std::vector<Constraint<ClockVariables>> &,const std::vector<std::vector<Value>> &)> &cost,
                                   const bool ignoreZero = false) : numOfClockVariables(boost::get_property(TA, boost::graph_num_of_vars)), dwellTimeClock(numOfClockVariables + 2), TA(TA), initStates(initStates), cost(cost) {
    DBM z = DBM::zero(numOfClockVariables + 1 + 2);
    // release Z(N+2)
    z.M = Bounds(std::numeric_limits<double>::infinity(), false);
    z.release(dwellTimeClock - 1);
    z.tightenWithoutClose(-1, dwellTimeClock - 1, {0, true});
    z.canonize();
    initialZone = std::move(z);
  }

  /*!
   * @brief feed one valuation with dwell time
   *
   * In this function, a piece of the entire piecewise constant function is fed and the matching ending in this piece is added to this->result.
   * See @ref outline-qtpm for the outline of the algorithm.
   *
   * @param [in] valuation The new signal valuation
   * @param [in] duration The duration of the given signal valuation
   * @note It is not a problem to give the same valuation consecutively.
   */
  void feed(const std::vector<Value> &valuation, const double duration) {

    for (auto &c: configuration) {
      // reset Z(N+2)
      c.first.zone.reset(dwellTimeClock - 1);
      // Check if the transition can fire at the beginning of the new piece of signal
      if (c.first.jumpable) {
        c.first.valuations.push_back(valuation);
        c.first.zone.elapse();
      }
    }

    // Add new configurations from initial states for the matching beginning from this piece of signal
    configuration.reserve(configuration.size() + initStates.size());
    for(const auto &q0: initStates) {
      configuration.emplace_back(BoostZoneGraphState<SignalVariables, ClockVariables, Value>{q0, false, initialZone, std::vector<std::vector<Value>>{}}, Weight::one());
    }

    // Conduct zone construction for time bound `duration` for the current signal valuation
    ZoneGraph ZG;
    std::unordered_map<ZGState, Weight> initStatesZG;
    zoneConstructionWithT(TA, configuration, cost, valuation, duration, ZG, initStatesZG);
#ifdef DEBUG
    assert(std::none_of(initStatesZG.begin(), initStatesZG.end(), [&](auto p) {
          return TA[ZG[p.first].vertex].isMatch;
        }));
#endif

    // Compute the accumulated weights, i.e., the quantitative semantics using the generalized Bellman-Ford algorithm
    std::unordered_map<ZGState, Weight> distance;
    bellman_ford<std::queue<ZGState>>(ZG, initStatesZG, distance);
    // This does not work when VerticesList is ListS. I do not know why.
    // write_graphviz(std::cerr, ZG, makeZoneGraphLabelWriter(ZG, TA, distance),make_weight_label_writer(ZG));
    configuration.clear();

    boost::unordered_map<ConfTuple_t, std::list<DBM>> confMap;

    // Construct the configuration just after the current piece
    for (auto w: distance) {
      // Ignore if the weight is already "zero"
      if (w.second == Weight::zero()) {
        continue;
      }
      // It is assumed that we do not have to use the configuration once we reach the accepting state.
      // ZG[w.first].zone.value.cols() > 0 corresponds to the requirement that we have non-zero time elapse in the current piece.
      if (!TA[ZG[w.first].vertex].isMatch && ZG[w.first].zone.value.cols() > 0) {
        auto z = ZG[w.first].zone;
        // Force dwellTimeClock == duration
        z.tightenWithoutClose(-1, dwellTimeClock - 1, Bounds{-duration, true});
        z.tightenWithoutClose(dwellTimeClock - 1, -1, Bounds{duration, true});
        if (z.isSatisfiable()) {
          auto it = confMap.find(std::make_tuple(ZG[w.first].vertex, ZG[w.first].jumpable, ZG[w.first].valuations, w.second));
          if (it != confMap.end()) {
            // Try to merge this zone to another zone at the same state
            for (DBM &zz: it->second) {
              if (zz.merge(z)) {
                goto next;
              }
            }
            // If the merging fails, we add this configuration
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

    // Put the resulting matching to this->result
    for (auto &w: distance) {
      if (w.second == Weight::zero()) {
        continue;
      }
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

    // Update absTime to the end of the current piece
    absTime += duration;
  }

  void getResult(boost::unordered_map<ResultMatrix, Weight> &v) const {
    v = result;
  }
  boost::unordered_map<ResultMatrix, Weight>& getResultRef()  {
    return result;
  }
};
