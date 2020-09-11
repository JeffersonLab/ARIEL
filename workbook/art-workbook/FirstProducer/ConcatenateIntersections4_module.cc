//
// Under development
//

#include "toyExperiment/MCDataProducts/IntersectionCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class ConcatenateIntersections4 : public art::EDProducer {

  public:

    explicit ConcatenateIntersections4(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;

  };

}

tex::ConcatenateIntersections4::ConcatenateIntersections4(fhicl::ParameterSet const& pset)
  : art::EDProducer{pset}
{
  produces<IntersectionCollection>();
}

void tex::ConcatenateIntersections4::produce( art::Event& event){

  std::vector<art::Handle<IntersectionCollection>> intersections;
  event.getManyByType(intersections);

  // Compute required size of the output std::vector.
  size_t total(0);
  for ( auto const& i : intersections ){
    total += i->size();
  }

  // Create empty data product and reserve the required size.
  std::unique_ptr<IntersectionCollection> out(new IntersectionCollection);
  out->reserve(total);

  // Copy input to output.
  for ( auto const& i : intersections ){
    out->insert( out->end(), i->begin(), i->end() );
  }

  // Put the output data product into the event.
  event.put( std::move(out) );


}

DEFINE_ART_MODULE(tex::ConcatenateIntersections4)
