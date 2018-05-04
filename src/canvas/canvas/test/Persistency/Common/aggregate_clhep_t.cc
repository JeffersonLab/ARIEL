#define BOOST_TEST_MODULE (ClhepProduct aggregation Test)
#include "canvas/test/Persistency/Common/MockRun.h"
#include "cetlib/quiet_unit_test.hpp"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
using CLHEP::HepVector;
using arttest::MockRun;

namespace {
  double constexpr tolerance = std::numeric_limits<double>::epsilon();
}

BOOST_AUTO_TEST_SUITE(aggregate_clhep_t)

BOOST_AUTO_TEST_CASE(hepvector)
{
  HepVector a(3, 0);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;

  HepVector b(3, 0);
  b[0] = 3;
  b[1] = 2;
  b[2] = 1;

  MockRun r;
  r.put<HepVector>(a);
  r.put<HepVector>(b);

  auto const& c = r.get<HepVector>();
  BOOST_CHECK_CLOSE_FRACTION(c[0], 4, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[1], 4, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[2], 4, tolerance);
}

BOOST_AUTO_TEST_CASE(hep2vector)
{
  MockRun r;
  r.put<Hep2Vector>(0, 3);
  r.put<Hep2Vector>(3, 5);
  auto const& c = r.get<Hep2Vector>();
  BOOST_CHECK_CLOSE_FRACTION(c[0], 3, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[1], 8, tolerance);
}

BOOST_AUTO_TEST_CASE(hep3vector)
{
  MockRun r;
  r.put<Hep3Vector>(0, 3, 19.2);
  r.put<Hep3Vector>(3, 5, 1.34);
  auto const& c = r.get<Hep3Vector>();
  BOOST_CHECK_CLOSE_FRACTION(c[0], 3, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[1], 8, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[2], 20.54, tolerance);
}

BOOST_AUTO_TEST_CASE(heplorentzvector)
{
  MockRun r;
  r.put<HepLorentzVector>(0, 3, 19.2, 6);
  r.put<HepLorentzVector>(3, 5, 1.34, 7);
  auto const& c = r.get<HepLorentzVector>();
  BOOST_CHECK_CLOSE_FRACTION(c[0], 3, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[1], 8, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[2], 20.54, tolerance);
  BOOST_CHECK_CLOSE_FRACTION(c[3], 13, tolerance);
}

BOOST_AUTO_TEST_CASE(hepmatrix)
{
  unsigned constexpr nrows{3};
  unsigned constexpr ncols{2};

  HepMatrix m1(nrows, ncols);
  m1(1, 1) = 1;
  m1(1, 2) = 0;
  m1(2, 1) = 0;
  m1(2, 2) = 1;
  m1(3, 1) = 1;
  m1(3, 2) = 1;

  HepMatrix m2(nrows, ncols);
  m2(1, 1) = 0;
  m2(1, 2) = 1;
  m2(2, 1) = 1;
  m2(2, 2) = 0;
  m2(3, 1) = 1;
  m2(3, 2) = 1;

  MockRun r;
  r.put<HepMatrix>(m1);
  r.put<HepMatrix>(m2);

  auto const& c = r.get<HepMatrix>();
  for (unsigned i{1}; i <= 2; ++i)
    for (unsigned j{1}; j <= 2; ++j)
      BOOST_CHECK_CLOSE_FRACTION((c(i, j)), 1, tolerance);
  BOOST_CHECK_CLOSE_FRACTION((c(3, 1)), 2, tolerance);
  BOOST_CHECK_CLOSE_FRACTION((c(3, 2)), 2, tolerance);
}

BOOST_AUTO_TEST_CASE(hepsymmatrix)
{
  unsigned constexpr n{2};
  MockRun r;
  HepSymMatrix m1(n);
  m1(1, 1) = 1;
  m1(1, 2) = 0;
  m1(2, 1) = 0;
  m1(2, 2) = 1;

  HepSymMatrix m2(n);
  m2(1, 1) = 0;
  m2(1, 2) = 1;
  m2(2, 1) = 1;
  m2(2, 2) = 0;

  r.put<HepSymMatrix>(m1);
  r.put<HepSymMatrix>(m2);

  auto const& c = r.get<HepSymMatrix>();
  for (unsigned i{1}; i <= n; ++i)
    for (unsigned j{1}; j <= n; ++j)
      BOOST_CHECK_CLOSE_FRACTION((c(i, j)), 1, tolerance);
}

BOOST_AUTO_TEST_SUITE_END()
