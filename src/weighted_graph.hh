#pragma once

#include <boost/graph/adjacency_list.hpp>

template<typename Weight>
using WeightedGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, Weight>>;

template<typename Base>
class MinPlusSemiring {
  inline MinPlusSemiring safe_sum(const MinPlusSemiring& x) const {
    const auto d = data + x.data;
    if (d < data && d < x.data) {
      return zero();
    } else {
      return MinPlusSemiring{d};
    }
  }
public:
  Base data;
  MinPlusSemiring (Base data = 0): data(data) {}
  MinPlusSemiring operator+(const MinPlusSemiring& x) const {
    return MinPlusSemiring{ std::min(data, x.data) };
  }
  void operator+=(const MinPlusSemiring& x) {
     data = std::min(data, x.data);
  }
  MinPlusSemiring operator*(const MinPlusSemiring& x) const {
    return safe_sum(x);
  }
  void operator*=(const MinPlusSemiring& x) {
    data = safe_sum(x).data;
  }
  bool operator!=(const MinPlusSemiring& x) const {
    return data != x.data;
  }
  static  MinPlusSemiring zero() {
    static MinPlusSemiring zero = MinPlusSemiring{ std::numeric_limits<Base>::has_infinity ?
                                                   std::numeric_limits<Base>::infinity():  
                                                   std::numeric_limits<Base>::max() };
    return zero;
  }
  static MinPlusSemiring one() {
    static MinPlusSemiring one = MinPlusSemiring{ 0 };
    return one;
  }
  MinPlusSemiring star() const {
    return one();
  }  
};

template<typename Base>
class MaxMinSemiring {
public:
  Base data;
  MaxMinSemiring (Base data = std::numeric_limits<Base>::infinity()): data(data) {}
  MaxMinSemiring operator+(const MaxMinSemiring& x) const {
    return MaxMinSemiring{ std::max(data, x.data) };
  }
  void operator+=(const MaxMinSemiring& x) {
     data = std::max(data, x.data);
  }
  MaxMinSemiring operator*(const MaxMinSemiring& x) const {
    return MaxMinSemiring{ std::min(data, x.data) };
  }
  void operator*=(const MaxMinSemiring& x) {
     data = std::min(data, x.data);
  }
  bool operator!=(const MaxMinSemiring& x) const {
    return data != x.data;
  }
  MaxMinSemiring star() const {
    return one();
  }
  static MaxMinSemiring one() {
    static MaxMinSemiring one = MaxMinSemiring{ std::numeric_limits<Base>::has_infinity ?
                                                std::numeric_limits<Base>::infinity():  
                                                std::numeric_limits<Base>::max() };
    return one;
  }
  static MaxMinSemiring zero() {
    static MaxMinSemiring zero = MaxMinSemiring{ std::numeric_limits<Base>::has_infinity ?
                                                 -std::numeric_limits<Base>::infinity():
                                                 std::numeric_limits<Base>::lowest() };
    return zero;
  }
};
