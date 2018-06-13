#pragma once

#include <vector>
#include <numeric>
#include "timed_automaton.hh"

template<class Weight, class Value, class ClockVariables>
Weight spaceRobustness(const std::vector<Constraint<ClockVariables>> &label, const std::vector<Value> valuation) {
  return std::accumulate(label.begin(), label.end(), Weight::one(), [&valuation] (Weight init, Constraint<ClockVariables> constraint) {
      Weight w = Weight::zero();
      switch (constraint.odr) {
      case Constraint<ClockVariables>::Order::lt:
      case Constraint<ClockVariables>::Order::le:
        w = Weight(constraint.c - valuation[constraint.x]);
        break;
      case Constraint<ClockVariables>::Order::ge:
      case Constraint<ClockVariables>::Order::gt:
        w = Weight(valuation[constraint.x] - constraint.c);
        break;
      }
      return init * w;
    });
}
