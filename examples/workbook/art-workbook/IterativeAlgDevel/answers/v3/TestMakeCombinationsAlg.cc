#include "art-workbook/IterativeAlgDevel/detail/FittedHelixDataFactory.h"
#include "art-workbook/IterativeAlgDevel/detail/ParticleInfoCollection.h"
// #include "art-workbook/IterativeAlgDevel/CombinationHelper.h"

#include <iostream>

namespace {

  using namespace helper;

  ParticleInfoCollection const pdt_ { "databaseFiles/pdt.fcl" };
  double const bz_   { 1.5 };
  double const mka_  { pdt_.getById( PDGCode::K_plus ).mass() };

}

int main()
{

}
