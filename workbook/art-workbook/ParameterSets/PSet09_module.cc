//
// Ninth exercise in the ParameterSets directory
//  - Encapsulate run-time bounds checking into
//    a function in an anonymous namespace.

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Utilities/Exception.h"

#include <iostream>
#include <vector>

namespace tex {

  class Pset09 : public art::EDAnalyzer {

  public:

    explicit Pset09(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    // Must lie on the half open interval [0,1)
    double weight_;

  };

}

namespace {

  // Helper function to encapsulate the validation of the weight parameter.
  // It returns a weight that is valid or throws an exception.
  double validWeight ( fhicl::ParameterSet const& pset ){

    double weight = pset.get<double>("weight");

    if ( weight < 0 || weight >= 1 ){
      throw art::Exception(art::errors::InvalidNumber)
        << "Weight parameter is out of bounds.  Weight is "
        << weight
        << "  Allowed range: is [0,1).\n";
    }

    return weight;
  }
}

tex::Pset09::Pset09(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  weight_(validWeight(pset)){
}

void tex::Pset09::analyze(art::Event const& event ){

  std::cout << "Event number: " << event.id()
            << "  weight: " << weight_
            << std::endl;
}

DEFINE_ART_MODULE(tex::Pset09)
