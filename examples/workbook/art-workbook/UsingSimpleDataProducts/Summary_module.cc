//
// Create the Event Summary data product.
//

#include "art-workbook/SimpleDataProducts/EventSummary.h"

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class Summary : public art::EDProducer {

  public:

    explicit Summary(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;

  private:

    art::InputTag fitsTag_;

  };

}

tex::Summary::Summary(fhicl::ParameterSet const& pset ):
  fitsTag_(pset.get<std::string>("fitsTag")){

  produces<EventSummary>();
}

void tex::Summary::produce( art::Event& event){

  // Get the input data products
  auto fits = event.getValidHandle<FittedHelixDataCollection>(fitsTag_);

  // Create empty output data produc.
  auto summary = std::make_unique<EventSummary>();

  for ( auto const& fit : *fits ){
    summary->increment( fit.helix().qgeo() );
  }
  std::cout << event.id() << " " << *summary << std::endl;

  event.put( std::move(summary) );

}

DEFINE_ART_MODULE(tex::Summary)
