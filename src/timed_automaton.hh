#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/optional.hpp>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "constraint.hh"
#include "constraint_driver.hh"

template<class U>
struct num_type_trait {
  using num_type = U;
};

template<>
struct num_type_trait<signed char> {
  using num_type = signed int;
};

template<>
struct num_type_trait<unsigned char> {
  using num_type = unsigned int;
};

struct Assign {
  std::size_t first, second;
};

static inline std::istream& operator>>(std::istream& is, Assign& p)
{
  if (is.get() != 'm') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }
  is >> p.first;

  if (!is) {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != ' ') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != ':') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != '=') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != ' ') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }
  
  if (is.get() != 'x') {
    abort();
    is.setstate(std::ios_base::failbit);
    return is;
  }
  is >> p.second;

  return is;
}

static inline std::ostream& operator<<(std::ostream& os, const Assign& p)
{
  os << 'm';
  os << p.first;

  os << " := x";

  os << p.second;

  return os;
}

namespace boost{
  enum vertex_match_t {vertex_match};
  enum edge_reset_t {edge_reset};
  enum edge_guard_t {edge_guard};
  enum graph_init_states_t {graph_init_states};
  enum graph_num_of_vars_t {graph_num_of_vars};
  enum graph_max_constraints_t {graph_max_constraints};
  enum graph_num_of_memories_t {graph_num_of_memories};

  BOOST_INSTALL_PROPERTY(vertex, match);
  BOOST_INSTALL_PROPERTY(edge, reset);
  BOOST_INSTALL_PROPERTY(edge, guard);
  BOOST_INSTALL_PROPERTY(graph, init_states);
  BOOST_INSTALL_PROPERTY(graph, num_of_vars);
  BOOST_INSTALL_PROPERTY(graph, max_constraints);
}

namespace {
  std::unique_ptr<ConstraintDriver> driverRef;
}

template<class ClockVariables>
static inline auto print(std::ostream& os, const typename Constraint<ClockVariables>::Order& odr) 
  -> typename std::conditional<true, std::ostream&, decltype(odr)>::type {
  switch (odr) {
  case Constraint<ClockVariables>::Order::lt:
    os << "<";
    break;
  case Constraint<ClockVariables>::Order::le:
    os << "<=";
    break;
  case Constraint<ClockVariables>::Order::ge:
    os << ">=";
    break;
  case Constraint<ClockVariables>::Order::gt:
    os << ">";
    break;
  }
  return os;
}

template<class ClockVariables>
static inline auto operator<<(std::ostream& os, const Constraint<ClockVariables>& p)
  -> typename std::conditional<true, std::ostream&, decltype(p)>::type 
{
  print<ClockVariables>(os << "x" << int(p.x) << " ", p.odr) << " " << p.c;
  return os;
}

template<class T>
static inline 
auto operator<<(std::ostream& os, const std::vector<T>& guard)
  -> typename std::conditional<true, std::ostream&, decltype(*guard.begin())>::type 
{
  bool first = true;
  os << "{";
  for (const auto &g: guard) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << static_cast<typename num_type_trait<T>::num_type>(g);
  }
  os << "}";
  return os;
}

template<class ClockVariables>
static inline std::istream& operator>>(std::istream& is, Constraint<ClockVariables>& p)
{
  if (is.get() != 'x') {
    is.setstate(std::ios_base::failbit);
    return is;
  }
  int x;
  is >> x;
  p.x = x;
  if (!is) {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != ' ') {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  char odr[2];
  is >> odr[0] >> odr[1];

  switch (odr[0]) {
  case '>':
    if (odr[1] == '=') {
      p.odr = Constraint<ClockVariables>::Order::ge;
      if (is.get() != ' ') {
        is.setstate(std::ios_base::failbit);
        return is;
      }
    } else if (odr[1] == ' ') {
      p.odr = Constraint<ClockVariables>::Order::gt;
    } else {
      is.setstate(std::ios_base::failbit);
      return is;
    }
    break;
  case '<':
    if (odr[1] == '=') {
      p.odr = Constraint<ClockVariables>::Order::le;
      if (is.get() != ' ') {
        is.setstate(std::ios_base::failbit);
        return is;
      }
    } else if (odr[1] == ' ') {
      p.odr = Constraint<ClockVariables>::Order::lt;
    } else {
      is.setstate(std::ios_base::failbit);
      return is;
    }
    break;
  default:
    is.setstate(std::ios_base::failbit);
    return is;
  }

  is >> p.c;
  return is;
}

template <class T>
static inline 
std::istream& operator>>(std::istream& is, std::vector<T>& resetVars)
{
  resetVars.clear();
  if (!is) {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (is.get() != '{') {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  if (!is) {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  while (true) {
    typename num_type_trait<T>::num_type x;
    is >> x;
    resetVars.emplace_back(std::move(x));
    if (!is) {
      is.setstate(std::ios_base::failbit);
      return is;
    }
    char c;
    is >> c;
    if (c == '}') {
      break;
    } else if (c == ',') {
      is >> c;
      if (c != ' ') {
        is.setstate(std::ios_base::failbit);
        return is;
      }
    } else {
      is.setstate(std::ios_base::failbit);
      return is;
    }
  }

  return is;
}

namespace boost {
  template <class T>
  static inline 
  std::ostream& operator<<(std::ostream& os, const boost::optional<T>& x)
  {
    if (x) {
      os << x.get();
    }
    else {
      os << "";
    }
    return os;
  }

  template <class T>
  static inline 
  std::istream& operator>>(std::istream& is, boost::optional<T>& x)
  {
    T result;
    if (is >> result) {
      x = result;
    }
    return is;
  }
}

template<class ClockVariables>
struct ResetVars {
  std::vector<ClockVariables> resetVars;
};

template<class ClockVariables>
static inline 
std::istream& operator>>(std::istream& is, ResetVars<ClockVariables>& resetVars)
{
  is >> resetVars.resetVars;
  return is;
}

template<class ClockVariables>
static inline 
std::ostream& operator<<(std::ostream& os, const ResetVars<ClockVariables>& resetVars)
{
  os << resetVars.resetVars;
  return os;
}

static inline 
std::istream& operator>>(std::istream& is, std::shared_ptr<BooleanConstraint>& guard)
{
  driverRef->parse(is);
  guard = driverRef->getResult();

  return is;
}

struct Assigns {
  std::vector<Assign> assignVars;
};

static inline 
std::istream& operator>>(std::istream& is, Assigns& assigns)
{
  is >> assigns.assignVars;
  return is;
}

static inline 
std::ostream& operator<<(std::ostream& os, const Assigns& assigns)
{
  os << assigns.assignVars;
  return os;
}

template<class SignalVariables>
struct BoostTAState {
  bool isInit;
  bool isMatch;
  std::vector<Constraint<SignalVariables>> label;
};

template<class ClockVariables>
struct BoostTATransition {
  //! @note this structure is necessary because of some problem in boost graph
  ResetVars<ClockVariables> resetVars;
  std::vector<Constraint<ClockVariables>> guard;
};

struct TAStateMemory {
  bool isInit;
  bool isMatch;
  std::shared_ptr<BooleanConstraint> label;
};

template<class ClockVariables, class MemoryVariables>
struct TATransitionMemory {
  //! @note this structure is necessary because of some problem in boost graph
  ResetVars<ClockVariables> resetVars;
  Assigns assigns;
  std::vector<Constraint<ClockVariables>> guard;
};

template<class SignalVariables, class ClockVariables>
using BoostTimedAutomaton = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, BoostTAState<SignalVariables>, BoostTATransition<ClockVariables>, 
                                                  boost::property<boost::graph_num_of_vars_t, std::size_t,
                                                                  boost::property<boost::graph_max_constraints_t, std::size_t>>>;

/*!
  @brief Timed Symbolic Automaton with memories 
*/
template<class SignalVariables, class ClockVariables, class MemoryVariables>
using TSAM = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, TAStateMemory, TATransitionMemory<ClockVariables, MemoryVariables>, 
                                   boost::property<boost::graph_num_of_vars_t, std::size_t,
                                                   boost::property<boost::graph_num_of_memories_t, std::size_t>>>;

template<class SignalVariables, class ClockVariables>
static inline 
void parseBoostTA(std::istream &file, BoostTimedAutomaton<SignalVariables, ClockVariables> &BoostTA,
                  std::vector<typename BoostTimedAutomaton<SignalVariables, ClockVariables>::vertex_descriptor> &initStates)
{

  boost::dynamic_properties dp(boost::ignore_other_properties);
  dp.property("match", boost::get(&BoostTAState<SignalVariables>::isMatch, BoostTA));
  dp.property("init",  boost::get(&BoostTAState<SignalVariables>::isInit, BoostTA));
  dp.property("label", boost::get(&BoostTAState<SignalVariables>::label, BoostTA));
  dp.property("reset", boost::get(&BoostTATransition<ClockVariables>::resetVars, BoostTA));
  dp.property("guard", boost::get(&BoostTATransition<ClockVariables>::guard, BoostTA));

  std::unique_ptr<ConstraintDriver> driver = std::make_unique<ConstraintDriver>();
  driverRef = std::move(driver);

  boost::read_graphviz(file, BoostTA, dp, "id");

  auto isMatchMap = boost::get(&BoostTAState<SignalVariables>::isInit, BoostTA);
  for (auto range = boost::vertices(BoostTA); range.first != range.second; range.first++) {
    auto q = *range.first;
    if (isMatchMap[q]) {
      initStates.push_back(q);
    }
  }

  std::size_t num_of_vars = 0;
  std::size_t max_constraints = 0;
  for (auto range = boost::edges(BoostTA); range.first != range.second; range.first++) {
    auto guard = BoostTA[*range.first].guard;
    for (auto g: guard) {
      num_of_vars = std::max<std::size_t>(num_of_vars, g.x + 1);
      max_constraints = std::max<std::size_t>(max_constraints, g.c);
    }
  }

  boost::set_property(BoostTA, boost::graph_max_constraints, max_constraints);
  boost::set_property(BoostTA, boost::graph_num_of_vars, num_of_vars);
}

template<class SignalVariables, class ClockVariables, class MemoryVariables>
static inline 
void parseTSAM(std::istream &file, TSAM<SignalVariables, ClockVariables, MemoryVariables> &BoostTA,
               std::vector<typename TSAM<SignalVariables, ClockVariables, MemoryVariables>::vertex_descriptor> &initStates)
{

  boost::dynamic_properties dp(boost::ignore_other_properties);
  dp.property("match", boost::get(&TAStateMemory::isMatch, BoostTA));
  dp.property("init",  boost::get(&TAStateMemory::isInit, BoostTA));
  dp.property("label", boost::get(&TAStateMemory::label, BoostTA));
  dp.property("reset", boost::get(&TATransitionMemory<ClockVariables, MemoryVariables>::resetVars, BoostTA));
  dp.property("assign", boost::get(&TATransitionMemory<ClockVariables, MemoryVariables>::assigns, BoostTA));
  dp.property("guard", boost::get(&TATransitionMemory<ClockVariables, MemoryVariables>::guard, BoostTA));

  boost::read_graphviz(file, BoostTA, dp);

  auto isMatchMap = boost::get(&TAStateMemory::isInit, BoostTA);
  for (auto range = boost::vertices(BoostTA); range.first != range.second; range.first++) {
    auto q = *range.first;
    if (isMatchMap[q]) {
      initStates.push_back(q);
    }
  }

  std::size_t num_of_vars = 0;
  std::size_t num_of_memories = 0;
  for (auto range = boost::edges(BoostTA); range.first != range.second; range.first++) {
    auto guard = BoostTA[*range.first].guard;
    for (const auto &g: guard) {
      num_of_vars = std::max<std::size_t>(num_of_vars, g.x + 1);
    }
    auto assigns = BoostTA[*range.first].assigns;
    for (const auto &assign: assigns.assignVars) {
      num_of_memories = std::max<std::size_t>(num_of_memories, assign.first + 1);
    }
  }

  boost::set_property(BoostTA, boost::graph_num_of_vars, num_of_vars);
  boost::set_property(BoostTA, boost::graph_num_of_vars, num_of_memories);
}
