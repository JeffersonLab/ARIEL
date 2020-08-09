//
// Create a new data product that is the union of the two Intersection
// data products.
//

#include "toyExperiment/MCDataProducts/IntersectionCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class ConcatenateIntersections1 : public art::EDProducer {

  public:

    explicit ConcatenateIntersections1(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;

  private:

    // Input tags for the two input data products.
    art::InputTag innerTag_;
    art::InputTag outerTag_;

  };

}

tex::ConcatenateIntersections1::ConcatenateIntersections1(fhicl::ParameterSet const& pset ):
  innerTag_(pset.get<std::string>("innerTag")),
  outerTag_(pset.get<std::string>("outerTag")){

  produces<IntersectionCollection>();
}

void tex::ConcatenateIntersections1::produce( art::Event& event){

  // Get the input data products
  auto inner = event.getValidHandle<IntersectionCollection>(innerTag_);
  auto outer = event.getValidHandle<IntersectionCollection>(outerTag_);

  // Create empty data product and reserve the required size.
  auto output = std::make_unique<IntersectionCollection>();
  output->reserve(inner->size()+outer->size());

  // Fill the data product
  output->insert( output->end(), inner->begin(), inner->end() );
  output->insert( output->end(), outer->begin(), outer->end() );

  // Add the product to the event
  event.put( std::move(output) );

}

DEFINE_ART_MODULE(tex::ConcatenateIntersections1)
