//
// Make a copy of an in-run data product.
//

#include "toyExperiment/MCDataProducts/MCRunSummary.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
//#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class ConcatenateIntersections5 : public art::EDProducer {

  public:

    explicit ConcatenateIntersections5(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;
    void endRun ( art::Run&   run  ) override;

  private:

    // Input tags for the two input data products.
    art::InputTag tag_;

  };

}

tex::ConcatenateIntersections5::ConcatenateIntersections5(fhicl::ParameterSet const& pset ):
  tag_(pset.get<std::string>("tag")){
  produces<MCRunSummary,art::InRun>();
}

void tex::ConcatenateIntersections5::produce( art::Event& ){
}

void tex::ConcatenateIntersections5::endRun( art::Run& run){

  // There is no run.ValidHandle<T>(); get the input data product the old way.
  art::Handle<MCRunSummary> summaryHandle;
  run.getByLabel(tag_,summaryHandle);

  // Create an output data product that is a copy of the input one.
  auto output = std::make_unique<MCRunSummary>(*summaryHandle);

  // Add the product to the run.
  run.put( std::move(output) );

}

DEFINE_ART_MODULE(tex::ConcatenateIntersections5)
