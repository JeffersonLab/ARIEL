//
// Illustate instance names and the Intersection class.
//

#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/Utilities/inputTagsFromStrings.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1D.h"
#include "TNtuple.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <vector>
#include <string>

namespace tex {

  class Intersections1 : public art::EDAnalyzer {

  public:

    explicit Intersections1(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void analyze(const art::Event& event) override;

  private:

    std::vector<art::InputTag> _intersectionTags;
    int _maxPrint;

    std::vector<TH1D*> _hIntersections;
    TH1D* _hShells;

    int _printCount;
  };

}

tex::Intersections1::Intersections1(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  _intersectionTags( inputTagsFromStrings( pset.get<std::vector<std::string>>("intersectionTags"))),
  _maxPrint( pset.get<int>("maxPrint",0) ),
  _printCount(0){
}


void tex::Intersections1::beginJob(){
  art::ServiceHandle<art::TFileService> tfs;

  _hNIntersections = tfs->make<TH1D>( "NIntersections", "Number of intersections per event", 100, 0., 100 );
  _hShells         = tfs->make<TH1D>( "Shells", "How often each Shell was hit", 15, 0., 15.);

  _nt = tfs->make<TNtuple>( "nt", "Intersections of tracks with shells", "evt:trk:x:y:z" );

}

void tex::Intersections1::analyze(const art::Event& event){

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

    for ( auto const& i: *intersections ){
      ntData[0] = event.id().event();
      ntData[1] = i.genTrack().key();
      ntData[2] = i.position().x();
      ntData[3] = i.position().y();
      ntData[4] = i.position().z();
      _nt->Fill( ntData );

      if ( doPrint ){
        std::cout << "Intersection: "
                  << n++ << " "
                  << i.genTrack().key()   << " "
                  << i.shell() << " "
                  << i.position()
                  << std::endl;
      }
    }
  }

  _hNIntersections->Fill( nIntersections );

}

DEFINE_ART_MODULE(tex::Intersections1)
