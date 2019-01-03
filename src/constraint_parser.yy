%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {Freezing}
%define parser_class_name {ConstraintParser}

%code requires{
   #include <memory>
   #include "complicated_constraint.hh"
   class ConstraintDriver;
   class ConstraintScanner;
}

%parse-param { ConstraintScanner  &scanner  }
%parse-param { ConstraintDriver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   #include "constraint_driver.hh"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token END 0  "end of file"
%token BLANK
%token <int>       INT
%token <std::size_t>       DVar
%token <std::size_t>       MVar
%token
  PLUS        "+"
  MINUS       "-"
  STAR        "*"
  LPAREN      "("
  RPAREN      ")"
  LT          "<"
  LE          "<="
  GE          ">="
  GT          ">"
  EQ          "=="
  COMMA       ","
  BEGIN_TOKEN "{"
;

%locations

%type <ComplicatedConstraint>          constraint
%type <std::shared_ptr<Expression>>   expr

%%

unit : BEGIN_TOKEN constraint_system END

constraint_system : constraint { driver.result = {$1}; }
                  | constraint_system COMMA constraint { driver.result.push_back(std::move($3)); }

/* The order matters */
constraint : expr LT expr { $$ = $1 < $3; }
           | expr GT expr { $$ = $1 > $3; }
           | expr LE expr { $$ = $1 <= $3; }
           | expr GE expr { $$ = $1 >= $3; }
           | expr EQ expr { $$ = $1 == $3; }

expr : expr STAR  expr { $$ = $1 * $3; }
     | expr PLUS  expr { $$ = $1 + $3; }
     | expr MINUS expr { $$ = $1 - $3; }
     | INT { $$ = std::make_shared<Expression>(Expression::kind_t::INT, $1); }
     | DVar { $$ = std::make_shared<Expression>(Expression::kind_t::DVAR, $1); }
     | MVar { $$ = std::make_shared<Expression>(Expression::kind_t::MVAR, $1); }

%%



void Freezing::ConstraintParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
