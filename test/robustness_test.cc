#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/robustness.hh"
#include "../src/weighted_graph.hh"

BOOST_AUTO_TEST_SUITE( SingleSpaceRobustnessTest )

typedef boost::mpl::list<MinPlusSemiring<int>, MaxMinSemiring<int>> testTypesInt;

template<class T>
struct ans_trait;

template<>
struct ans_trait<MinPlusSemiring<int>> {
  const static int ans = 40;
};

template<>
struct ans_trait<MaxMinSemiring<int>> {
  const static int ans = 10;
};

BOOST_AUTO_TEST_CASE_TEMPLATE( SpaceRobustNessTest0, Weight, testTypesInt )
{
  using C_t = std::vector<Constraint<uint8_t>>;
  using V_t = std::vector<int>;
  using CM_t = ConstraintMaker<uint8_t>;

  C_t label = {
    CM_t(0) > 100,
    CM_t(1) < 30
  };
  V_t valuation = {130, 20};
  Weight result = singleSpaceRobustness<Weight>(label, valuation);

  constexpr int const ans = ans_trait<Weight>::ans;

  BOOST_CHECK_EQUAL(result.data, ans);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( MultipleSpaceRobustnessTest )

typedef boost::mpl::list<MinPlusSemiring<int>, MaxMinSemiring<int>> testTypesInt;

template<class T>
struct ans_trait;

template<>
struct ans_trait<MinPlusSemiring<int>> {
  const static int ans = 40 + 70;
};

template<>
struct ans_trait<MaxMinSemiring<int>> {
  const static int ans = 10;
};

BOOST_AUTO_TEST_CASE_TEMPLATE( SpaceRobustNessTest0, Weight, testTypesInt )
{
  using C_t = std::vector<Constraint<uint8_t>>;
  using V_t = std::vector<std::vector<int>>;
  using CM_t = ConstraintMaker<uint8_t>;

  C_t label = {
    CM_t(0) > 100,
    CM_t(1) < 30
  };
  V_t valuations = {{130, 20}, {150, 10}};
  Weight result = multipleSpaceRobustness<Weight>(label, valuations);

  constexpr int const ans = ans_trait<Weight>::ans;

  BOOST_CHECK_EQUAL(result.data, ans);
}
BOOST_AUTO_TEST_SUITE_END()
