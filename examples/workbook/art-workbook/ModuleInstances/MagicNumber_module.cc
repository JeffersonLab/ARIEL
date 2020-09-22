//
//  First exercise in the ModuleInstances directory
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>

namespace tex {

  class MagicNumber : public art::EDAnalyzer {

  public:

    explicit MagicNumber(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    int magicNumber_;

  };

}

tex::MagicNumber::MagicNumber(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  magicNumber_(pset.get<int>("magicNumber")){

  std::cout << "MagicNumber::constructor: "
            << " magic number: " << magicNumber_
            << std::endl;

}

void tex::MagicNumber::analyze(art::Event const& event ){

  std::cout << "MagicNumber::analyze: event: " << event.id()
            << " magic number: "         << magicNumber_
            << std::endl;

}

DEFINE_ART_MODULE(tex::MagicNumber)
