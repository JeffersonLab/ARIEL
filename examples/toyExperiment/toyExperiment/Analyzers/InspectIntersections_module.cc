//
// Inspect the MC truth about the intersections between tracks and detector shells.
//

#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/Utilities/inputTagsFromStrings.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "TH1F.h"
#include "TNtuple.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <vector>
#include <string>

namespace tex {

  class InspectIntersections : public art::EDAnalyzer {

  public:

    explicit InspectIntersections(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void analyze(const art::Event& event) override;

  private:

    std::vector<art::InputTag> _intersectionTags;
    int _maxPrint;

    art::ServiceHandle<art::TFileService> _tfs;

    TH1F* _hNIntersections;

    TNtuple* _nt;

    int _printCount;
  };

}

tex::InspectIntersections::InspectIntersections(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  _intersectionTags( inputTagsFromStrings( pset.get<std::vector<std::string>>("intersectionTags"))),
  _maxPrint( pset.get<int>("maxPrint",0) ),
  _tfs( art::ServiceHandle<art::TFileService>() ),
  _printCount(0){
}


void tex::InspectIntersections::beginJob(){
  _hNIntersections = _tfs->make<TH1F>( "NIntersections", "Number of intersections per event", 100, 0., 100 );
  _nt = _tfs->make<TNtuple>( "nt", "Intersections of tracks with shells", "evt:trk:x:y:z" );
}

void tex::InspectIntersections::analyze(const art::Event& event){

  bool doPrint( _printCount++ < _maxPrint );
  if ( doPrint ){
    std::cout << "\nHit Summary for Event: " << event.id() << std::endl;
  }

  int nIntersections(0);

  float ntData[5];
  int n(0);
  for ( auto const& tag : _intersectionTags ){
    auto intersections = event.getValidHandle<IntersectionCollection>(tag);

    nIntersections += intersections->size();

    for ( auto const& hit: *intersections ){
      ntData[0] = event.id().event();
      ntData[1] = hit.genTrack().key();
      ntData[2] = hit.position().x();
      ntData[3] = hit.position().y();
      ntData[4] = hit.position().z();
      _nt->Fill( ntData );

      if ( doPrint ){
        std::cout << "Hit: "
                  << n++ << " "
                  << hit.genTrack().key()   << " "
                  << hit.shell() << " "
                  << hit.position()
                  << std::endl;
      }
    }
  }

  _hNIntersections->Fill( nIntersections );

}

DEFINE_ART_MODULE(tex::InspectIntersections)
