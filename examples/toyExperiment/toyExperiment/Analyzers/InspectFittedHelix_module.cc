//
//  Look at the contents of the FittedHelixDataCollection; make both printout
//  and histograms.
//

#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#ifdef ART_ROOT_IO
#include "art_root_io/TFileService.h"
#else
#include "art/Framework/Services/Optional/TFileService.h"
#endif

#include "TH1F.h"
#include "TNtuple.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <string>

namespace tex {

  class InspectFittedHelix : public art::EDAnalyzer {

  public:
    explicit InspectFittedHelix(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void analyze(const art::Event& event) override;

  private:
    art::InputTag _fitsTag;
    int _maxPrint;

    art::ServiceHandle<art::TFileService> _tfs;

    int _printCount;

    TH1F* _hNFits;
  };

}

tex::InspectFittedHelix::InspectFittedHelix(fhicl::ParameterSet const& pset)
  : art::EDAnalyzer(pset)
  , _fitsTag(pset.get<std::string>("fitsTag"))
  , _maxPrint(pset.get<int>("maxPrint", 0))
  , _tfs(art::ServiceHandle<art::TFileService>())
  , _printCount(0)
{}

void
tex::InspectFittedHelix::beginJob()
{
  _hNFits =
    _tfs->make<TH1F>("NFits", "Number of fitted tracks per event", 20, 0., 20);
}

void
tex::InspectFittedHelix::analyze(const art::Event& event)
{

  // Get the generated particles from the event.
  auto fits = event.getValidHandle<FittedHelixDataCollection>(_fitsTag);

  _hNFits->Fill(fits->size());

  // Printout, if requested.
  bool doPrint = (++_printCount < _maxPrint);
  if (doPrint) {
    std::cout << "\nNumber of FittedHelices in event " << event.id().event()
              << " is: " << fits->size() << std::endl;
  }

  for (auto const& fit : *fits) {
    if (doPrint) {
      std::cout << fit << std::endl;
    }
  }
}

DEFINE_ART_MODULE(tex::InspectFittedHelix)
