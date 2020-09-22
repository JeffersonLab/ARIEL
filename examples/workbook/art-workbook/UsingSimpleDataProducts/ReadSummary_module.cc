//
// Read the EventSummary data product.
//

#include "art-workbook/SimpleDataProducts/EventSummary.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>
#include <memory>

namespace tex {

  class ReadSummary : public art::EDAnalyzer {

  public:

    explicit ReadSummary(fhicl::ParameterSet const& pset);

    void analyze( art::Event const& event) override;

  private:

    art::InputTag summaryTag_;

  };

}

tex::ReadSummary::ReadSummary(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  summaryTag_(pset.get<std::string>("summaryTag")){
}

void tex::ReadSummary::analyze( art::Event const& event){

  auto summary = event.getValidHandle<EventSummary>(summaryTag_);
  std::cout << event.id() << " " << *summary << std::endl;

}

DEFINE_ART_MODULE(tex::ReadSummary)
