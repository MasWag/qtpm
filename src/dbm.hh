#pragma once
// (setq flycheck-clang-language-standard "c++14")
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <boost/unordered_map.hpp>

#include "constraint.hh"

using Bounds = std::pair<double, bool>;
static inline Bounds
operator+ (const Bounds &a,const Bounds &b) {
  return Bounds(a.first + b.first, a.second && b.second);
}
static inline Bounds
operator- (const Bounds &a,const Bounds &b) {
  return Bounds(a.first - b.first, a.second && b.second);
}
static inline void
operator+= (Bounds &a, const Bounds b) {
  a.first += b.first;
  a.second = a.second && b.second;
}
namespace std {
  static inline std::ostream &operator<<(std::ostream &os, const Bounds &b) {
    os << "(" << b.first << ", " << b.second << ")";
    return os;
  }
}
static inline Bounds
operator+ (Bounds a, const double b) {
  a.first += b;
  return a;
}
static inline Bounds
operator- (Bounds a, const double b) {
  a.first -= b;
  return a;
}


#include <eigen3/Eigen/Core>
//! @todo configure include directory for eigen

/*!
 * @brief Implementation of a zone with DBM DBM
 * For the detail of DBMs, see for example @cite BY03.
 *
 * @note Internally, the variable 0 is used for the constant while externally, the actual clock variable is 0 origin, i.e., the variable 0 for the user is the variable 1 internally. So, we need increment or decrement to fill the gap.
 */
struct DBM {
  using Tuple = std::tuple<std::vector<Bounds>,Bounds>;
  //! @brief The matrix representing the DBM
  Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic> value;
  //! @brief The threshold for the normalization
  Bounds M;

  /*!
   * @brief Returns the number of the variables represented by this zone
   * @returns The number of the variables
   */
  inline std::size_t getNumOfVar() const {
    return value.cols() - 1;
  }

  inline void cutVars (std::shared_ptr<DBM> &out,std::size_t from,std::size_t to) const {
    out = std::make_shared<DBM>();
    out->value.resize(to - from + 2, to - from + 2);
    out->value.block(0,0,1,1) << Bounds(0,true);
    out->value.block(1, 1, to - from + 1, to - from + 1) = value.block(from + 1, from + 1, to - from + 1,to - from + 1);
    out->value.block(1, 0, to - from + 1, 1) = value.block(from + 1, 0, to - from + 1, 1);
    out->value.block(0, 1, 1, to - from + 1) = value.block(0, from + 1, 1, to - from + 1);
    out->M = M;
  }
  

  //! @brief Make the zone of size `size` such that all the values are zero
  static DBM zero(int size) {
    static DBM zeroZone;
    if (zeroZone.value.cols() == size) {
      return zeroZone;
    }
    zeroZone.value.resize(size, size);
    zeroZone.value.fill(Bounds(0, true));
    return zeroZone;
  }


  //! @brief Return the tuple representation of the DBM.
  std::tuple<std::vector<Bounds>,Bounds> toTuple() const {
    // omit (0,0)
    return std::tuple<std::vector<Bounds>,Bounds>(std::vector<Bounds>(value.data() + 1, value.data() + value.size()),M);
  }

  //! @brief add the constraint x - y <= (c,s) but does not close.
  //! @note The result is not canonized
  void tightenWithoutClose(uint8_t x, uint8_t y, Bounds c) {
    x++;
    y++;
    value(x,y) = std::min(value(x, y), c);
  }

  //! @brief add the constraint \f$x - y \le (c,s)\f$
  void tighten(uint8_t x, uint8_t y, Bounds c) {
    x++;
    y++;
    value(x,y) = std::min(value(x, y), c);
    close1(x);
    close1(y);
  }

  void close1(uint8_t x) {
    for (int i = 0; i < value.rows(); i++) {
      value.row(i) = value.row(i).array().min(value.row(x).array() + value(i, x));
      // for (int j = 0; j < value.cols(); j++) {
      //   value(i, j) = std::min(value(i, j), value(i, x) + value(x, j));
      // }
    }
  }
  
  // The reset value is always (0, \le)
  void reset(uint8_t x) {
    // DBM orig = *this;
    // 0 is the special varibale here
    x++;
    value(0,x) = Bounds(0, true);
    value(x,0) = Bounds(0, true);
    value.col(x).tail(value.rows() - 1) = value.col(0).tail(value.rows() - 1);
    value.row(x).tail(value.cols() - 1) = value.row(0).tail(value.cols() - 1);
    // This close is necessary to keep canonized.
    close1(x);
    // if (orig.isCanonized()) {
    //   assert(isCanonized());
    // }
  }
  
  //! @note the result is not canonized
  void release(uint8_t x) {
    static constexpr Bounds infinity = Bounds(std::numeric_limits<double>::infinity(), false);
    // 0 is the special varibale here
    x++;
    value.col(x).fill(infinity);
    value.row(x).fill(infinity);
    value(0,x) = Bounds(0, true);
    value(x,0) = infinity;
  }

  /*!
   * @brief Assign the strongest post-condition of the delay
   *
   * @note We do not allow time elapse of duration zero
   */
  void elapse() {
    // DBM orig = *this;
    static constexpr Bounds infinity = Bounds(std::numeric_limits<double>::infinity(), false);
    value.col(0).fill(infinity);
    for (int i = 0; i < value.row(0).size(); ++i) {
      value.row(0)[i].second = false;
    }
    // if (orig.isCanonized()) {
    //   assert(isCanonized());
    // }
  }

  /*!
   * @brief make the zone canonical
   */
  void canonize() {
    for (int k = 0; k < value.cols(); k++) {
      close1(k);
    }
  }

  /*!
   * @brief check if the zone is satisfiable
   *
   * @pre The zone is canonical
   */
  inline bool isSatisfiableWithoutCanonize() const {
    return (value + value.transpose()).minCoeff() >= Bounds(0.0, true);
  }

  /*!
   * @brief check if the zone is satisfiable
   */
  bool isSatisfiable() {
    canonize();
    return isSatisfiableWithoutCanonize();
  }

  /*!
   * @brief truncate the constraints compared with a constant greater than or equal to M
  */
  void abstractize() {
    static constexpr Bounds infinity = Bounds(std::numeric_limits<double>::infinity(), false);
    for (auto it = value.data(); it < value.data() + value.size(); it++) {
      if (*it >= M) {
        *it = infinity;
      }
    }
  }

  /*!
   * @brief make the zone unsatisfiable
   */
  void makeUnsat() {
    value(0, 0) = Bounds(-std::numeric_limits<double>::infinity(), false);
  }

  bool operator== (DBM z) const {
    z.value(0,0) = value(0,0);
    return value == z.value;
  }

  void operator&=(const DBM &z) {
    value.array() = value.array().min(z.value.array());
    canonize();
  }

  bool operator>(const DBM &z) const {
    return (value.array() > z.value.array()).all();
  }

  bool operator<=(const DBM &z) const {
    return !isSatisfiableWithoutCanonize() || (value.array() <= z.value.array()).all();
  }

  /*!
   * @brief Make the convex union of two DBMs
   *
   * The convex union is the smallest DBM containing the given DBMs.
   *
   * @param[in] z The DBM to take the convex union
   * @param[out] dest The DBM to write the resulting convex union
   *
   * @pre getNumOfVar() == z.getNumOfVar() == dest.getNumOfVar()
   */
  void convexUnion(const DBM &z, DBM &dest) const {
    dest.value.array() = value.array().max(z.value.array());
  }

  /*!
   * @brief Try to merge the given DBM to this DBM
   *
   * *this is updated to the convex union of *this and z if it is the union of them. This happens if and only if one of them includes the other or two zones are adjacent.
   *
   * @param[in] z The DBM to merge
   * @retval true when the convex union is the union
   * @retval false when the convex union is not the union
   */
  bool merge(const DBM &z) {
    // When *this is included by z
    if (*this <= z) {
      value = std::move(z.value);
      return true;
    }
    // When z is included by *this
    if (z <= *this) {
      return true;
    }

    // Take the convex union
    DBM convex = DBM::zero(getNumOfVar()+ 1);
    convexUnion(z, convex);

    // Check if convex is the union of *this and z
    const std::size_t N = value.cols();
    for (std::size_t i = 0; i < N; i++) {
      for (std::size_t j = 0; j < N; j++) {
        // we do not have to look at the diags
        if (i == j) {
          continue;
        }
        // Exclude the values in *this and check if it is included to z
        const Bounds rev = {-value(j,i).first, !value(j,i).second};
        // When we do not have to tighten, we skip
        if (convex.value(i,j) <= rev) {
          continue;
        }
        DBM c = convex;
        c.value(i,j) = rev;
        c.close1(i);
        c.close1(j);
        if (!(c <= z)) {
          return false;
        }
      }
    }

    value =  std::move(convex.value);
    return true;
  }

  bool isCanonized() const {
    DBM tmp = *this;
    tmp.canonize();
    return !tmp.isSatisfiableWithoutCanonize() || value == tmp.value;
  }
};

// struct ZoneAutomaton : public AbstractionAutomaton<DBM> {
//   struct TAEdge {
//     State source;
//     State target;
//     Alphabet c;
//     std::vector<Alphabet> resetVars;
//     std::vector<Constraint> guard;
//   };

//   boost::unordered_map<std::tuple<State, State, Alphabet>, TAEdge> edgeMap;
//   boost::unordered_map<std::pair<TAState, typename DBM::Tuple>, RAState> zones_in_za;
//   int numOfVariables;
// };

// static inline std::ostream& operator << (std::ostream& os, const DBM& z) {
//   for (int i = 0; i < z.value.rows();i++) {
//     for (int j = 0; j < z.value.cols();j++) {
//       os << z.value(i,j);
//     }
//     os << "\n";
//   }
//   os << std::endl;
//   return os;
// }


