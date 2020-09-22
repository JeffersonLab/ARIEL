//
// Filter that returns true if the event number is odd.
//

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>

namespace tex {

  class OddEventNumber : public art::EDFilter {

  public:

    explicit OddEventNumber(fhicl::ParameterSet const& );

    bool filter(art::Event& event) override;

  };

}

tex::OddEventNumber::OddEventNumber(fhicl::ParameterSet const& ){
}

bool tex::OddEventNumber::filter(art::Event& event ){
  bool isOdd = event.id().event()%2 == 1;
  std::cout << "Event: "
            << event.id()
            << ((isOdd) ? " Pass" : " Fail")
            << std::endl;
  return isOdd;
}

DEFINE_ART_MODULE(tex::OddEventNumber)
