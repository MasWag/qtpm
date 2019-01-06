#pragma once
#include <ostream>
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
    assert(kind == kind_t::INT || kind == kind_t::DVAR || kind == kind_t::MVAR);
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
  std::size_t max_var() { 
    switch (kind) {
    case kind_t::INT:
      return 0;
    case kind_t::DVAR: 
    case kind_t::MVAR: 
      return variable;
    case kind_t::PLUS: 
    case kind_t::MINUS: 
    case kind_t::TIMES:
      return std::max(children.first->max_var(), children.second->max_var());
    }
    abort();
    return 0;
  };
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
  ComplicatedConstraint(std::shared_ptr<Expression> first,
                        kind_t kind,
                        std::shared_ptr<Expression> second) :first(first), kind(kind), second(second) {}
  std::size_t max_var() { 
    return std::max(first->max_var(), second->max_var());
  };
};

static inline std::shared_ptr<ComplicatedConstraint>
operator<(std::shared_ptr<Expression> first,
          std::shared_ptr<Expression> second) {
  return std::make_shared<ComplicatedConstraint>(first, ComplicatedConstraint::kind_t::LT, second);
}
static inline std::shared_ptr<ComplicatedConstraint> operator<=(std::shared_ptr<Expression> first,
                                                                std::shared_ptr<Expression> second) {
  return std::make_shared<ComplicatedConstraint>(first, ComplicatedConstraint::kind_t::LE, second);
}
static inline std::shared_ptr<ComplicatedConstraint> operator==(std::shared_ptr<Expression> first,
                                                                std::shared_ptr<Expression> second) {
  return std::make_shared<ComplicatedConstraint>(first, ComplicatedConstraint::kind_t::EQ, second);
}
static inline std::shared_ptr<ComplicatedConstraint> operator>=(std::shared_ptr<Expression> first,
                                                                std::shared_ptr<Expression> second) {
  return std::make_shared<ComplicatedConstraint>(first, ComplicatedConstraint::kind_t::GE, second);
}
static inline std::shared_ptr<ComplicatedConstraint> operator>(std::shared_ptr<Expression> first,
                                                               std::shared_ptr<Expression> second) {
  return std::make_shared<ComplicatedConstraint>(first, ComplicatedConstraint::kind_t::GT, second);
}

static inline std::ostream& operator<<(std::ostream& os, const ComplicatedConstraint &cs) {
  os << cs.first;
  switch (cs.kind) {
  case ComplicatedConstraint::kind_t::LT:
    os << " < ";
    break;
  case ComplicatedConstraint::kind_t::LE:
    os << " <= ";
    break;
  case ComplicatedConstraint::kind_t::EQ:
    os << " == ";
    break;
  case ComplicatedConstraint::kind_t::GE:
    os << " >= ";
    break;
  case ComplicatedConstraint::kind_t::GT:
    os << " > ";
    break;
  }
  os << cs.second;  
  return os;
}

/*!
  @brief Boolean Constraint at the states
*/
struct BooleanConstraint {
  enum class kind_t {ATOM, AND, OR, NOT} kind;
  union {
    std::shared_ptr<ComplicatedConstraint> atom;
    std::shared_ptr<BooleanConstraint> child;
    std::pair<std::shared_ptr<BooleanConstraint>, std::shared_ptr<BooleanConstraint>> children;
  };
  BooleanConstraint(kind_t kind, std::shared_ptr<ComplicatedConstraint> atom) :kind(kind), atom(std::move(atom)) {
    assert(kind == kind_t::ATOM);
  }

  BooleanConstraint(std::shared_ptr<BooleanConstraint> first,
                    kind_t kind,
                    std::shared_ptr<BooleanConstraint> second) :kind(kind), children(first, second) {
    assert(kind == kind_t::AND || kind == kind_t::OR);
  }

  BooleanConstraint(kind_t kind,
                    std::shared_ptr<BooleanConstraint> child) :kind(kind), child(child) {
    assert(kind == kind_t::NOT);
  }

  ~BooleanConstraint() {
    switch (kind) {
    case kind_t::ATOM:
      atom.reset();
      break;
    case kind_t::AND: 
    case kind_t::OR: 
      children.~pair();
      break;
    case kind_t::NOT:
      child.reset();
      break;
    }
  }
};

static inline std::shared_ptr<BooleanConstraint> operator&&(std::shared_ptr<BooleanConstraint> first,
                                                            std::shared_ptr<BooleanConstraint> second) {
  return std::make_shared<BooleanConstraint>(first, BooleanConstraint::kind_t::AND, second);
}
static inline std::shared_ptr<BooleanConstraint> operator||(std::shared_ptr<BooleanConstraint> first,
                                                            std::shared_ptr<BooleanConstraint> second) {
  return std::make_shared<BooleanConstraint>(first, BooleanConstraint::kind_t::OR, second);
}
static inline std::shared_ptr<BooleanConstraint> operator!(std::shared_ptr<BooleanConstraint> child) {
  return std::make_shared<BooleanConstraint>(BooleanConstraint::kind_t::NOT, child);
}

static inline 
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<BooleanConstraint> guard)
{
  os << guard;

  return os;
}

static inline 
std::ostream& operator<<(std::ostream& os, const BooleanConstraint::kind_t kind)
{
  switch (kind) {
  case BooleanConstraint::kind_t::ATOM:
    break;
  case BooleanConstraint::kind_t::AND: 
    os << "&&";
    break;
  case BooleanConstraint::kind_t::OR: 
    os << "||";
    break;
  case BooleanConstraint::kind_t::NOT:
    os << "!";
    break;
  }

  return os;
}

static inline 
std::ostream& operator<<(std::ostream& os, const ComplicatedConstraint::kind_t kind)
{
  switch (kind) {
  case ComplicatedConstraint::kind_t::LT: 
    os << "<";
    break;
  case ComplicatedConstraint::kind_t::LE:
    os << "<=";
    break;
  case ComplicatedConstraint::kind_t::EQ: 
    os << "==";
    break;
  case ComplicatedConstraint::kind_t::GE:
    os << ">=";
    break;
  case ComplicatedConstraint::kind_t::GT:
    os << ">";
    break;
  }

  return os;
}

static inline 
std::ostream& operator<<(std::ostream& os, const Expression::kind_t kind)
{
  switch (kind) {
  enum class kind_t {INT, DVAR, MVAR, PLUS, MINUS, TIMES} kind;
  case Expression::kind_t::INT: 
    break;
  case Expression::kind_t::DVAR:
    os << "x";
    break;
  case Expression::kind_t::MVAR: 
    os << "m";
    break;
  case Expression::kind_t::PLUS: 
    os << "+";
    break;
  case Expression::kind_t::MINUS: 
    os << "-";
    break;
  case Expression::kind_t::TIMES:
    os << "*";
    break;
  }

  return os;
}
