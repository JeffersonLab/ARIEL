//
// Eighth exercise in the ParameterSet directory
//  - Use of art::Exception to respond to an invalid parameter.
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Utilities/Exception.h"

#include <iostream>
#include <vector>

namespace tex {

  class PSet08 : public art::EDAnalyzer {

  public:

    explicit PSet08(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    // Must lie on the half open interval [0,1)
    double weight_;

  };

}

tex::PSet08::PSet08(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  weight_(pset.get<double>("weight")){

  if ( weight_ < 0 || weight_ >= 1 ){
    throw art::Exception(art::errors::InvalidNumber)
      << "The weight parameter is out of bounds.  Weight is "
      << weight_
      << "  Allowed range: is [0,1).\n";
  }
}

void tex::PSet08::analyze(art::Event const& event ){

  std::cout << "Event number: " << event.id()
            << "  weight: " << weight_
            << std::endl;


}

DEFINE_ART_MODULE(tex::PSet08)
