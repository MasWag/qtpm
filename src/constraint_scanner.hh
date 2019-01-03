#pragma once

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "../build/constraint_parser.tab.hh"
#include "../build/location.hh"

class ConstraintScanner : public yyFlexLexer {
public:
   
  ConstraintScanner(std::istream *in) : yyFlexLexer(in) {};
  virtual ~ConstraintScanner() {};

  //get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual
  int yylex( Freezing::ConstraintParser::semantic_type * const lval, 
             Freezing::ConstraintParser::location_type *location );

private:
  Freezing::ConstraintParser::semantic_type *yylval = nullptr;
};

