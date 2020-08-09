//
// Not really a producer.  Has printout to show when it is called.
//

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class PseudoProducer : public art::EDProducer {

  public:

    explicit PseudoProducer(fhicl::ParameterSet const& pset);

    void produce( art::Event& event) override;

  private:

    std::string label_;

  };

}

tex::PseudoProducer::PseudoProducer(fhicl::ParameterSet const& pset ):
  label_(pset.get<std::string>("module_label")){
}

void tex::PseudoProducer::produce( art::Event& event){

  std::cout << "PseudoProducer "
            << label_
            << ": "
            << event.id()
            << std::endl;
}

DEFINE_ART_MODULE(tex::PseudoProducer)
