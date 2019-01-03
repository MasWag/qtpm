#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>

/*!
  @brief Expressions at the states

  @note This class provides "eval" function. But users can also use user-defined eval in their cost function.
 */
struct Expression {
  enum class kind_t {INT, DVAR, MVAR, PLUS, MINUS, TIMES} kind;
  union {
    int constant;
    std::size_t variable;
    std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>> children;
  };
  Expression(kind_t kind, std::size_t variable) :kind(kind), variable(variable) {
    assert(kind == kind_t::DVAR || kind == kind_t::MVAR);
  }

  Expression(kind_t kind,
             std::shared_ptr<Expression> first,
             std::shared_ptr<Expression> second) :kind(kind), children(first, second) {
    assert(kind == kind_t::PLUS || kind == kind_t::MINUS || kind == kind_t::TIMES);
  }

  ~Expression() {
    switch (kind) {
    case kind_t::INT:
    case kind_t::DVAR: 
    case kind_t::MVAR: 
      break;
    case kind_t::PLUS: 
    case kind_t::MINUS: 
    case kind_t::TIMES:
      children.~pair();
    }
  }

  double eval(const std::vector<double> &dval, const std::vector<double> &mval) {
    switch (kind) {
    case kind_t::INT:
      return constant;
    case kind_t::DVAR: 
      return dval.at(variable);
      break;
    case kind_t::MVAR: 
      return mval.at(variable);
      break;
    case kind_t::PLUS: 
      return children.first->eval(dval, mval) + children.second->eval(dval, mval);
      break;
    case kind_t::MINUS: 
      return children.first->eval(dval, mval) - children.second->eval(dval, mval);
      break;
    case kind_t::TIMES:
      return children.first->eval(dval, mval) * children.second->eval(dval, mval);
      break;
    }
  }
};

static inline std::shared_ptr<Expression> operator+(std::shared_ptr<Expression> first,
                                             std::shared_ptr<Expression> second) {
  return std::make_shared<Expression>(Expression::kind_t::PLUS, first, second);
}

static inline std::shared_ptr<Expression> operator-(std::shared_ptr<Expression> first,
                                             std::shared_ptr<Expression> second) {
  return std::make_shared<Expression>(Expression::kind_t::MINUS, first, second);
}

static inline std::shared_ptr<Expression> operator*(std::shared_ptr<Expression> first,
                                             std::shared_ptr<Expression> second) {
  return std::make_shared<Expression>(Expression::kind_t::TIMES, first, second);
}

struct ComplicatedConstraint {
  std::shared_ptr<Expression> first;
  enum class kind_t {LT, LE, EQ, GE, GT} kind;
  std::shared_ptr<Expression> second;
};

static inline ComplicatedConstraint operator<(std::shared_ptr<Expression> first,
                                       std::shared_ptr<Expression> second) {
  return {first, ComplicatedConstraint::kind_t::LT, second};
}
static inline ComplicatedConstraint operator<=(std::shared_ptr<Expression> first,
                                        std::shared_ptr<Expression> second) {
  return {first, ComplicatedConstraint::kind_t::LE, second};
}
static inline ComplicatedConstraint operator==(std::shared_ptr<Expression> first,
                                        std::shared_ptr<Expression> second) {
  return {first, ComplicatedConstraint::kind_t::EQ, second};
}
static inline ComplicatedConstraint operator>=(std::shared_ptr<Expression> first,
                                        std::shared_ptr<Expression> second) {
  return {first, ComplicatedConstraint::kind_t::GE, second};
}
static inline ComplicatedConstraint operator>(std::shared_ptr<Expression> first,
                                       std::shared_ptr<Expression> second) {
  return {first, ComplicatedConstraint::kind_t::GT, second};
}
