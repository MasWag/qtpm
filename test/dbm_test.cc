#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/dbm.hh"

BOOST_AUTO_TEST_SUITE(DBMTest)

BOOST_AUTO_TEST_CASE( ReleaseTest )
{
  DBM orig, released;
   orig = DBM::zero(3);

  // 1 \le x \le 2
  // 5 \le y \le 4
  // unsatisfiable
  orig.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-5, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{4, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  released = orig;
  released.release(1);

  orig.canonize();
  released.canonize();
  BOOST_TEST(!orig.isSatisfiable());
  BOOST_TEST(released.isSatisfiable());  

  // The released variable should be non-negative.
  released.tighten(1, -1, Bounds{-1, true});
  BOOST_TEST(!released.isSatisfiable());  
}

BOOST_AUTO_TEST_CASE( InclusionTest )
{
  DBM A, B;
  A = DBM::zero(3);
  B = DBM::zero(3);

  // 1 \le x \le 2
  // 5 \le y \le 4
  // unsatisfiable
  A.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  B.value <<
    Bounds{0, true}, Bounds{0, true}, Bounds{-4, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  BOOST_TEST((A <= B));
  BOOST_TEST(!(A > B));

  B.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  BOOST_TEST((A <= B));

  B.value <<
    Bounds{0, true}, Bounds{-2, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  BOOST_TEST(!(A <= B));
}

BOOST_AUTO_TEST_CASE( ConvexUnionTest )
{
  DBM A, B, C, result;
  A = DBM::zero(3);
  B = DBM::zero(3);
  C = DBM::zero(3);
  result = DBM::zero(3);  

  // 1 \le x \le 2
  // 5 \le y \le 4
  A.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-5, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{4, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  // 3 \le x \le 4
  // 5 \le y \le 4
  B.value <<
    Bounds{0, true}, Bounds{-3, true}, Bounds{-5, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{4, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  // 1 \le x \le 4
  // 5 \le y \le 4
  result.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-5, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{4, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  A.convexUnion(B, C);
  BOOST_TEST((C == result));
}


BOOST_AUTO_TEST_CASE( MergeTest )
{
  DBM A, B;
  A = DBM::zero(3);
  B = DBM::zero(3);

  ////  Merged because A <= B  /////

  // 1 \le x \le 2
  // 4 \le y \le 5
  A.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  B.value <<
    Bounds{0, true}, Bounds{0, true}, Bounds{-4, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  DBM Bold = B;

  BOOST_TEST(A.merge(B));
  BOOST_TEST((A.toTuple() == Bold.toTuple()));

  ////  Merged because A >= B  /////

  A.value <<
    Bounds{0, true}, Bounds{0, true}, Bounds{-4, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  B.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  DBM Aold = A;

  BOOST_TEST(A.merge(B));
  BOOST_TEST((A.toTuple() == Aold.toTuple()));

  ////  Merged because convex_hull(A, B) == A | B  /////

  // 1 \le x \le 2
  // 4 \le y \le 5
  A.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  // 2 \le x \le 3
  // 4 \le y \le 5
  B.value <<
    Bounds{0, true}, Bounds{-2, true}, Bounds{-4, true}, \
    Bounds{3, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  DBM result;
  result = DBM::zero(3);

  // 1 \le x \le 3
  // 4 \le y \le 5
  result.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{3, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};  

  BOOST_TEST(A.merge(B));
  BOOST_TEST((A.toTuple() == result.toTuple()));

  ////  Not Merged because convex_hull(A, B) != A | B  /////

  // 1 \le x \le 2
  // 4 \le y \le 5
  A.value <<
    Bounds{0, true}, Bounds{-1, true}, Bounds{-4, true}, \
    Bounds{2, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};
  // 3 \le x \le 4
  // 4 \le y \le 5
  B.value <<
    Bounds{0, true}, Bounds{-3, true}, Bounds{-4, true}, \
    Bounds{4, true},  Bounds{0, true}, Bounds{std::numeric_limits<double>::infinity(), true}, \
    Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), true}, Bounds{0, true};

  Aold = A;
  Bold = B;

  BOOST_TEST(!A.merge(B));

  // When the merge failed, the input should not changed.
  BOOST_TEST((A.toTuple() == Aold.toTuple()));
  BOOST_TEST((B.toTuple() == Bold.toTuple()));
}

// BOOST_AUTO_TEST_CASE( findDurationTest )
// {
//   DBM orig, guard;
//   orig = DBM::zero(3);
//   guard = DBM::zero(3);

//   // 1 \le x \le 2
//   // 3 \le y \le 4
//   orig.value << \
//     Bounds(0, true), Bounds(-1, true), Bounds(-3,true), \
//     Bounds(2, true), Bounds(0, true), Bounds(std::numeric_limits<double>::infinity(), false), \
//     Bounds(4, true), Bounds(std::numeric_limits<double>::infinity(), false), Bounds(0, true);

//   // 7 \le x \le 8
//   // 9 \le y \le 10
//   guard.value << \
//     Bounds(0, true), Bounds(-7, true), Bounds(-9,true), \
//     Bounds(8, true), Bounds(0, true), Bounds(std::numeric_limits<double>::infinity(), false), \
//     Bounds(10, true), Bounds(std::numeric_limits<double>::infinity(), false), Bounds(0, true);

//   Bounds lower, upper;
//   orig.findDuration(guard, lower, upper);

//   BOOST_CHECK_EQUAL(lower.first, -5);
//   BOOST_CHECK_EQUAL(lower.second, true);
//   BOOST_CHECK_EQUAL(upper.first, 7);
//   BOOST_CHECK_EQUAL(upper.second, true);
// }

// BOOST_AUTO_TEST_CASE( findDurationTest2 )
// {
//   DBM orig, guard;
//   orig = DBM::zero(2);
//   guard = DBM::zero(2);

//   // 3 < x < 5
//   orig.value << Bounds(0, true), Bounds(-3, false), Bounds(5, false), Bounds(0, true);

//   // 4 < x < 6
//   guard.value << Bounds(0, true), Bounds(-4, false), Bounds(6, false), Bounds(0, true);

//   Bounds lower, upper;
//   orig.findDuration(guard, lower, upper);

//   BOOST_CHECK_EQUAL(lower.first, 0);
//   BOOST_CHECK_EQUAL(lower.second, false);
//   BOOST_CHECK_EQUAL(upper.first, 3);
//   BOOST_CHECK_EQUAL(upper.second, false);
// }

// BOOST_AUTO_TEST_CASE( constraintToDBMTest )
// {
//   DBM guard;
//   guard = DBM::zero(2);

//   constraintsToDBM({TimedAutomaton::X(0) < 10}, guard);

//   BOOST_CHECK_EQUAL(guard.value(1, 0).first, 10);
//   BOOST_CHECK_EQUAL(guard.value(1, 0).second, false);
// }

BOOST_AUTO_TEST_SUITE_END()
