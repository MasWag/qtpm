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

BOOST_AUTO_TEST_SUITE_END()
