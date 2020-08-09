//
// Select events that have one or more good combinations.
//

#include "art-workbook/ExDataProducts/CombinationCollection.h"
#include "art-workbook/ExUtilities/PSetChecker.h"
#include "art-workbook/ExN/TrackCuts.h"

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/Conditions/Conditions.h"

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "cetlib/exception.h"

#include <string>

namespace tex {

  class SelectCombinations : public art::EDFilter {

  public:

    explicit SelectCombinations(fhicl::ParameterSet const& pset);

    bool beginRun( art::Run&   run   ) override;
    bool filter  ( art::Event& event ) override;

  private:

    std::string combinerTag_;
    int         maxPrint_;
    TrackCuts   cuts_;

    art::ServiceHandle<Geometry>          geom_;
    art::ServiceHandle<Conditions>        conditions_;

    // Not available until beginRun time.
    double bz_;

    int printCount_;

    // Helper functions.
    void checkParameterSet( fhicl::ParameterSet const& pset );

  };

}

bool tex::SelectCombinations::beginRun( art::Run& ){
  bz_ = geom_->bz();
  return true;
}

tex::SelectCombinations::SelectCombinations(fhicl::ParameterSet const& pset):
  combinerTag_( pset.get<std::string>        ("combinerInputTag") ),
  maxPrint_(    pset.get<int>                ("maxPrint",0) ),
  cuts_(        pset.get<fhicl::ParameterSet>("cuts")),
  geom_(),
  conditions_(),
  bz_(0.),
  printCount_(0){

  checkParameterSet(pset);

}

bool tex::SelectCombinations::filter( art::Event& event){

  auto combos = event.getValidHandle<CombinationCollection>(combinerTag_);

  int ngood(0);
  for ( auto const& combo : *combos ){

    FittedHelixData const& fit0(*combo.child(0));
    FittedHelixData const& fit1(*combo.child(1));

    if ( fit0.hits().size() < size_t(cuts_.minHitsPerTrack) ) continue;
    if ( fit1.hits().size() < size_t(cuts_.minHitsPerTrack) ) continue;

    if ( fit0.helix().p(bz_) < cuts_.pmin ) continue;
    if ( fit1.helix().p(bz_) < cuts_.pmin ) continue;

    ++ngood;

  }

  return ( ngood > 0 );

} // end filter

void tex::SelectCombinations::checkParameterSet( fhicl::ParameterSet const& pset ){

  std::vector<std::string> knownParameterNames = { "combinerInputTag", "maxPrint", "cuts" };
  PSetChecker checker( knownParameterNames, pset );
  if ( checker.bad() ){
    throw cet::exception("PSET") << checker.message("SelectCombinations") << "\n";
  }
}

DEFINE_ART_MODULE(tex::SelectCombinations)
