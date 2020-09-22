//
// Reject events with too few hits.
//

#include "art-workbook/ExUtilities/PSetChecker.h"

#include "toyExperiment/RecoDataProducts/TrkHitCollection.h"

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "cetlib/exception.h"

#include <string>

namespace tex {

  class MinimumHits : public art::EDFilter {

  public:

    explicit MinimumHits(fhicl::ParameterSet const& pset);

    bool filter ( art::Event& event ) override;

  private:

    art::InputTag makerTag_;
    size_t        minimumHits_;

    // Helper functions.
    void checkParameterSet( fhicl::ParameterSet const& pset );

  };

}

tex::MinimumHits::MinimumHits(fhicl::ParameterSet const& pset):
  makerTag_(    pset.get<std::string>("makerInputTag") ),
  minimumHits_( pset.get<int>        ("minimumHits")      )
{
  checkParameterSet(pset);
}

bool tex::MinimumHits::filter( art::Event& event){

  auto hits = event.getValidHandle<TrkHitCollection>(makerTag_);

  return (hits->size() >= minimumHits_);
}

void tex::MinimumHits::checkParameterSet( fhicl::ParameterSet const& pset ){

  std::vector<std::string> knownParameterNames = { "makerTag_", "minimumHits" };
  PSetChecker checker( knownParameterNames, pset );
  if ( checker.bad() ){
    throw cet::exception("PSET") << checker.message("MinimumHits") << "\n";
  }
}

DEFINE_ART_MODULE(tex::MinimumHits)
