#include "art-workbook/IterativeAlgDevel/detail/FittedHelixDataFactory.h"
#include "art-workbook/IterativeAlgDevel/detail/ParticleInfoCollection.h"
#include "art-workbook/IterativeAlgDevel/CombinationHelper.h"

#include <iostream>

namespace {

  using namespace helper;

  ParticleInfoCollection const pdt_ { "databaseFiles/pdt.fcl" };
  double const bz_   { 1.5 };
  double const mka_  { pdt_.getById( PDGCode::K_plus ).mass() };

  std::vector<std::size_t> failedTests;
  std::size_t testNo{};

  void test_case(std::size_t const npos,
                 std::size_t const nneg,
                 std::size_t const reference) // no. of expected combinations
  {
    ++testNo;
    auto const coll = FittedHelixDataFactory::make(npos,nneg);
    std::vector<CombinationData> test = makeCombinationsAlg(coll, mka_, bz_);
    if ( test.size() != reference ) failedTests.push_back( testNo );
  }

}

int main()
{

  test_case(0,0,0);
  test_case(0,1,0);
  test_case(1,0,0);
  test_case(17,0,0);
  test_case(0,17,0);
  test_case(1,1,1);
  test_case(4,9,36);

  int rc{};
  for ( std::size_t const i : failedTests ) {
    std::cout << "Test " << i << " failed\n";
    rc = i;
  }
  return rc;

}
