//
//  Inspect the simulated hits for the tracker.
//

#include "toyExperiment/Conditions/Conditions.h"
#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/RecoDataProducts/TrkHitCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "canvas/Persistency/Common/FindOne.h"

#include "TH1F.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

namespace tex {

  class InspectTrkHits : public art::EDAnalyzer {

  public:

    explicit InspectTrkHits(fhicl::ParameterSet const& pset);

    void beginRun( const art::Run& run ) override;
    void analyze(const art::Event& event) override;

  private:

    art::InputTag _hitsTag;
    int         _maxPrint;

    art::ServiceHandle<Geometry>   _geom;
    art::ServiceHandle<Conditions> _conditions;

    art::ServiceHandle<art::TFileService> _tfs;

    TH1F* _hNHits;
    TH1F* _hDzInner;
    TH1F* _hDzOuter;

    int _printCount
#ifdef __clang__
    [[gnu::unused]]
#endif
    ;
  };

}

tex::InspectTrkHits::InspectTrkHits(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  _hitsTag( pset.get<std::string>("hitMakerTag") ),
  _maxPrint( pset.get<int>("maxPrint",0) ),
  _geom(),
  _conditions(),
  _tfs( art::ServiceHandle<art::TFileService>() ),
  _printCount(0){
}


void tex::InspectTrkHits::beginRun( const art::Run& ){

  // The tracker geometry and conditions are only defined during a run.
  // They are not defined at begin-job time.

  // Choose an inner and an outer detector.
  int inner = _geom->tracker().shells().front().id();
  int outer = _geom->tracker().shells().back().id();

  // Set limits of the delta z plot, different for inner vs outer.
  double zIn  = 6.*_conditions->shellConditions(inner).sigmaZ();
  double zOut = 6.*_conditions->shellConditions(outer).sigmaZ();

  _hNHits   = _tfs->make<TH1F>( "NHits", "Number of tracker hits per event",   80,   0.,    80 );
  _hDzInner = _tfs->make<TH1F>( "DzInner", "z (smeared-gen) for Inner shells", 100, -zIn,   zIn );
  _hDzOuter = _tfs->make<TH1F>( "DzOuter", "z (smeared-gen) for Outer shells", 100, -zOut, zOut );

}

void tex::InspectTrkHits::analyze(const art::Event& event){

  Tracker const& tracker(_geom->tracker());

  art::Handle<TrkHitCollection> hitsHandle;
  event.getByLabel( _hitsTag, hitsHandle );
  TrkHitCollection const& hits(*hitsHandle);

  _hNHits->Fill( hits.size() );

  // Navigator object to match hits with their truth information.
  art::FindOne<Intersection> fa(hitsHandle, event, _hitsTag);

  for ( size_t i=0; i<hits.size(); ++i) {

    TrkHit      const& hit  ( hits.at(i) );
    Intersection const& truth( fa.at(i).ref() );

    double dz = hit.z()-truth.position().z();
    if ( tracker.shell( hit.shell() ).trackerComponent() == TrackerComponent::inner ) {
      _hDzInner->Fill(dz);
    }else{
      _hDzOuter->Fill(dz);
    }

  }

}

DEFINE_ART_MODULE(tex::InspectTrkHits)
