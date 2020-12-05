#include <boost/test/unit_test.hpp>
#include "../src/weighted_graph.hh"

BOOST_AUTO_TEST_SUITE(SemiringTest)

BOOST_AUTO_TEST_CASE( MinPlusSemiringTest0 )
{
  BOOST_CHECK_EQUAL((MinPlusSemiring<int>(-2) + MinPlusSemiring<int>(2)).data, -2);
  BOOST_CHECK_EQUAL((MinPlusSemiring<int>(-2) * MinPlusSemiring<int>(2)).data, 0);
  BOOST_CHECK_EQUAL((MinPlusSemiring<int>(-2) + MinPlusSemiring<int>::zero()).data, -2);
  BOOST_CHECK_EQUAL((MinPlusSemiring<int>(-2) * MinPlusSemiring<int>::one()).data, -2);
  BOOST_CHECK_EQUAL((MinPlusSemiring<int>(-2).star()).data, INT_MIN);
}

BOOST_AUTO_TEST_CASE( MaxMinSemiringTest0 )
{
  BOOST_CHECK_EQUAL((MaxMinSemiring<int>(-2) + MaxMinSemiring<int>(2)).data, 2);
  BOOST_CHECK_EQUAL((MaxMinSemiring<int>(-2) * MaxMinSemiring<int>(2)).data, -2);
  BOOST_CHECK_EQUAL((MaxMinSemiring<int>(-2) + MaxMinSemiring<int>::zero()).data, -2);
  BOOST_CHECK_EQUAL((MaxMinSemiring<int>(-2) * MaxMinSemiring<int>::one()).data, -2);
  BOOST_CHECK_EQUAL((MaxMinSemiring<int>(-2).star()).data, MaxMinSemiring<int>::one().data);
}
BOOST_AUTO_TEST_SUITE_END()
