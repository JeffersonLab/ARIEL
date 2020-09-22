//
// Create the Event Summary data product.
//

#include "art-workbook/SimpleDataProducts/EventSummary.h"
#include "art-workbook/SimpleDataProducts/TrackSummary.h"

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class Summary2 : public art::EDProducer {

  public:

    explicit Summary2(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;

  private:

    art::InputTag fitsTag_;

  };

}

tex::Summary2::Summary2(fhicl::ParameterSet const& pset ):
  fitsTag_(pset.get<std::string>("fitsTag")){

  produces<EventSummary>();
}

namespace {

  void fillTrackSummary ( tex::FittedHelixData const& fit, tex::TrackSummary& sum){

    // Outer most shell number of the set of inner shells (0 based counting).
    static const int innerShellMax{4};

    for ( auto const& hitPtr : fit.hits() ){
      auto const& hit(*hitPtr);
      if ( hit.shell() <= innerShellMax ){
        sum.incrementInner();
      } else{
        sum.incrementOuter();
      }

    }
  }
}

void tex::Summary2::produce( art::Event& event){

  // Get the input data products
  auto fits = event.getValidHandle<FittedHelixDataCollection>(fitsTag_);

  // Create empty output data produc.
  auto summary = std::make_unique<EventSummary>();

  for ( auto const& fit : *fits ){
    summary->increment( fit.helix().qgeo() );
    TrackSummary trkSummary;
    fillTrackSummary( fit, trkSummary);
    std::cout << "Track: " << trkSummary << std::endl;
  }
  std::cout << event.id() << " " << *summary << std::endl;

  event.put( std::move(summary) );

}

DEFINE_ART_MODULE(tex::Summary2)
