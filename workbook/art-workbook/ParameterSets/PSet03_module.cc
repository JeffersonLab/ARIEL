//
// Third exercise in the ParameterSets directory
//  - Introduce optional parameters
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>
#include <vector>

namespace tex {

  class PSet03 : public art::EDAnalyzer {

  public:

    explicit PSet03(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    int debugLevel_;
    std::vector<double> g_;

  };

}

tex::PSet03::PSet03(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  debugLevel_(pset.get<int>("debugLevel",0)),
  g_(         pset.get<std::vector<double>>("g", std::vector<double>(5,1.0))){

  std::cout << "\n\ndebug level: " << debugLevel_ << std::endl;
  std::cout << "g: ";
  for ( double a : g_){
    std::cout << " " << a;
  }
  std::cout << std::endl;

}

void tex::PSet03::analyze(art::Event const& ){
}

DEFINE_ART_MODULE(tex::PSet03)
