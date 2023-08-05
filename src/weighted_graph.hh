#pragma once

#include <boost/graph/adjacency_list.hpp>

template<typename Weight>
using WeightedGraph = boost::adjacency_list<boost::listS, boost::listS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, Weight>>;

template<typename Base>
class MinPlusSemiring {
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
    return MinPlusSemiring( data + x.data);
  }
  void operator*=(const MinPlusSemiring& x) {
    data += x;
  }
  bool operator!=(const MinPlusSemiring& x) const {
    return data != x.data;
  }
  bool operator==(const MinPlusSemiring& x) const {
    return data == x.data;
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
    if (data < 0) {
      return MinPlusSemiring{ std::numeric_limits<Base>::has_infinity ?
                               -std::numeric_limits<Base>::infinity():  
                               std::numeric_limits<Base>::min() };
    } else {
      return one();
    }
  }  
};

template<typename Base>
std::size_t hash_value(MinPlusSemiring<Base> const& v) {
  return boost::hash_value(v.data);
}

template<typename Base>
class MaxPlusSemiring {
public:
  Base data;
  MaxPlusSemiring (Base data = 0): data(data) {}
  MaxPlusSemiring operator+(const MaxPlusSemiring& x) const {
    return MaxPlusSemiring{ std::max(data, x.data) };
  }
  void operator+=(const MaxPlusSemiring& x) {
     data = std::max(data, x.data);
  }
  MaxPlusSemiring operator*(const MaxPlusSemiring& x) const {
    return MaxPlusSemiring{data + x.data};
  }
  void operator*=(const MaxPlusSemiring& x) {
    data += x;
  }
  bool operator!=(const MaxPlusSemiring& x) const {
    return data != x.data;
  }
  bool operator==(const MaxPlusSemiring& x) const {
    return data == x.data;
  }
  static  MaxPlusSemiring zero() {
    static MaxPlusSemiring zero = MaxPlusSemiring{ std::numeric_limits<Base>::has_infinity ?
                                                   -std::numeric_limits<Base>::infinity():  
                                                   std::numeric_limits<Base>::min() };
    return zero;
  }
  static MaxPlusSemiring one() {
    static MaxPlusSemiring one = MaxPlusSemiring{ 0 };
    return one;
  }
  MaxPlusSemiring star() const {
    if (data > 0) {
      return MaxPlusSemiring{ std::numeric_limits<Base>::has_infinity ?
                               std::numeric_limits<Base>::infinity():  
                               std::numeric_limits<Base>::max() };
    } else {
      return one();
    }
  }
};

template<typename Base>
std::size_t hash_value(MaxPlusSemiring<Base> const& v) {
  return boost::hash_value(v.data);
}

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
  bool operator==(const MaxMinSemiring& x) const {
    return data == x.data;
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

template<class Base>
std::ostream& operator<<(std::ostream& os, const MaxMinSemiring<Base>& s) {
  os << s.data;
  return os;
}

template<typename Base>
std::size_t hash_value(MaxMinSemiring<Base> const& v) {
  return boost::hash_value(v.data);
}

class BooleanSemiring {
public:
  bool data;
  BooleanSemiring (double value): data(value > 0) {}
  BooleanSemiring (bool data = false): data(data) {}
  BooleanSemiring operator+(const BooleanSemiring& x) const {
    return BooleanSemiring{ data || x.data };
  }
  void operator+=(const BooleanSemiring& x) {
     data = data || x.data;
  }
  BooleanSemiring operator*(const BooleanSemiring& x) const {
    return BooleanSemiring( data && x.data);
  }
  void operator*=(const BooleanSemiring& x) {
    data = data && x.data;
  }
  bool operator!=(const BooleanSemiring& x) const {
    return data != x.data;
  }
  bool operator==(const BooleanSemiring& x) const {
    return data == x.data;
  }
  static  BooleanSemiring zero() {
    static BooleanSemiring zero = BooleanSemiring{ false };
    return zero;
  }
  static BooleanSemiring one() {
    static BooleanSemiring one = BooleanSemiring{ true };
    return one;
  }
  BooleanSemiring star() const {
    if (data == true) {
      return one();
    } else {
      return zero();
    }
  }  
};

static inline std::size_t hash_value(BooleanSemiring const& v) {
  return boost::hash_value(v.data);
}
