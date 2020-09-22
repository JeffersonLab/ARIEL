//
// Fourth exercise in the ParameterSets directory
//  - Introduce type conversions
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

namespace tex {

  class PSet04 : public art::EDAnalyzer {

  public:

    explicit PSet04(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  };

}

tex::PSet04::PSet04(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset){

  std::string sa = pset.get<std::string>("a");
  std::cout << "parameter a as a string: " << sa << std::endl;

  double a = pset.get<double>("a");
  std::cout << "parameter a as a double: " <<  a << std::endl;

  int ia = pset.get<int>("a");
  std::cout << "parameter a as int:      " << ia << std::endl;

  std::string sb = pset.get<std::string>("b");
  std::cout << "parameter b as a string: " << sb << std::endl;

  double b = pset.get<double>("b");
  std::cout << "parameter b as a double: " <<  b << std::endl;

  // Increase the precsion of the printout of doubles to 10 signficant figures.
  std::streamsize savedPrecision = std::cout.precision(10);

  std::cout << "parameter b as a double with more significant figures: "
            << b << std::endl;

  // Restore the old precision.
  std::cout.precision(savedPrecision);

  // If c has a fractional part, this will generate a run-time error on conversion to int.
  std::string sc = pset.get<std::string>("c");
  std::cout << "parameter c as a string: " << sc << std::endl;
  int ic = pset.get<int>("c");
  std::cout << "parameter c as an int:   " << ic << std::endl;

}

void tex::PSet04::analyze(art::Event const& ){
}

DEFINE_ART_MODULE(tex::PSet04)
