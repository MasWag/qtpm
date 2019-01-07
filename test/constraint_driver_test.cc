#include <boost/test/unit_test.hpp>
#include <sstream>
#include "../src/constraint_driver.hh"

BOOST_AUTO_TEST_SUITE(ConstraintDriverTest)

struct ConstraintDriverFixture {
  ConstraintDriver driver;
  std::stringstream stream;
  std::string expectedResult;
  void parse() {
    std::stringstream result;
    driver.parse(stream);
    const std::shared_ptr<BooleanConstraint> resultPtr = driver.getResult();
    result << *(resultPtr);
    BOOST_CHECK_EQUAL(result.str(), expectedResult);
  }
};

struct SimpleConstraint : public ConstraintDriverFixture {
  SimpleConstraint() {
    stream << "x0 > 100";
    expectedResult = "x0>100";
  }
};


BOOST_AUTO_TEST_SUITE(parse)
BOOST_FIXTURE_TEST_CASE(simpleConstraint, SimpleConstraint)
{
  parse();
}

struct ComplicatedConstraint : public ConstraintDriverFixture {
  ComplicatedConstraint() {
    stream << "x0 < 100 && x0 * x1 > x0 + x1";
    expectedResult = "x0<100&&x0*x1>x0+x1";
  }
};


BOOST_FIXTURE_TEST_CASE(complicatedConstraint, ComplicatedConstraint)
{
  parse();
}

// BOOST_FIXTURE_TEST_CASE(simpleTimedParse, SimpleTimedExpression)
// {
//   TREDriver driver;
//   std::stringstream result;
//   driver.parse(stream);
//   result << *(driver.getResult());
//   BOOST_CHECK_EQUAL(result.str(), "((a(ba))%((1, 0),(2, 0)))");
// }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
