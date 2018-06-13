#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

//! @brief The return values of comparison of two values. Similar to strcmp.
enum class Order {
  LT, EQ, GT
};

inline static bool toBool (Order odr) {
  return odr == Order::EQ;
}

//! @brief A constraint in a guard of transitions
template<class ClockVariables>
struct Constraint {
  enum class Order {
    lt,le,ge,gt
  };

  ClockVariables x;
  Order odr;
  int c;

  bool satisfy (double d) const {
    switch (odr) {
    case Order::lt:
      return d < c;
    case Order::le:
      return d <= c;
    case Order::gt:
      return d > c;
    case Order::ge:
      return d >= c;
    }
    return false;
  }
  using Interpretation = std::vector<double>;
  ::Order operator() (Interpretation val) const {
    if (satisfy (val.at (x))) {
      return ::Order::EQ;
    } else if (odr == Order::lt || odr == Order::le) {
      return ::Order::GT;
    } else {
      return ::Order::LT;
    }
  }
};

// An interface to write an inequality constrait easily
template<class ClockVariables>
class ConstraintMaker {
  ClockVariables x;
public:
  ConstraintMaker(ClockVariables x) : x(x) {}
  Constraint<ClockVariables> operator<(int c) {
    return Constraint<ClockVariables> {x, Constraint<ClockVariables>::Order::lt, c};
  }
  Constraint<ClockVariables> operator<=(int c) {
    return Constraint<ClockVariables> {x, Constraint<ClockVariables>::Order::le, c};
  }
  Constraint<ClockVariables> operator>(int c) {
    return Constraint<ClockVariables> {x, Constraint<ClockVariables>::Order::gt, c};
  }
  Constraint<ClockVariables> operator>=(int c) {
    return Constraint<ClockVariables> {x, Constraint<ClockVariables>::Order::ge, c};
  }
};

/*!
  @brief remove any inequality x > c or x >= c
 */
template<class ClockVariables>
static inline void widen(std::vector<Constraint<ClockVariables>> &guard) {
  guard.erase(std::remove_if(guard.begin(), guard.end(), [](Constraint<ClockVariables> g) {
        return g.odr == Constraint<ClockVariables>::Order::ge || g.odr == Constraint<ClockVariables>::Order::gt;
      }), guard.end());
}
