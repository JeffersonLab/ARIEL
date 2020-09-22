//
//  Exercise ?:
//  Illustrate TFileDirectory and how to write out a TGraph.
//

#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/Utilities/inputTagsFromStrings.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "TDirectory.h"
#include "TGraph.h"
#include "TH1D.h"

#include <iostream>
#include <string>
#include <vector>

namespace tex {

  class RootPart2 : public art::EDAnalyzer {

  public:

    explicit RootPart2(fhicl::ParameterSet const& );

    void beginJob() override;
    void analyze(art::Event const& event) override;

  private:

    std::vector<art::InputTag> intersectionTags_;

    TH1D*   hNIntersections_;
    TGraph* gIntersections_;

    size_t gIntersectionsCount_;

  };

}

tex::RootPart2::RootPart2(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),

  intersectionTags_( inputTagsFromStrings( pset.get<std::vector<std::string>>("intersectionTags"))),

  hNIntersections_(nullptr),
  gIntersections_(nullptr),
  gIntersectionsCount_(0){
}

void tex::RootPart2::beginJob(){

  art::ServiceHandle<art::TFileService> tfs;

  // This is pure ROOTology - nothing to do with art.
  gIntersections_ = tfs->make<TGraph>();
  gIntersections_->SetName("gIntersections");
  gIntersections_->SetTitle("Y vs X for Intersections");
  gDirectory->Append(gIntersections_ );

  // Make a subdirectory and cd to it; new histograms etc will be created here.
  art::TFileDirectory const& dir = tfs->mkdir("subDirectory");
  hNIntersections_  = dir.make<TH1D>( "hNIntersections_",
                                      "Number of Intersections per event",
                                      100, 0., 100.);

}

void tex::RootPart2::analyze(art::Event const& event ){

  size_t totalIntersections(0);

  for ( auto const& tag : intersectionTags_ ){
    auto intersections = event.getValidHandle<IntersectionCollection>(tag);

    totalIntersections += intersections->size();

    for ( auto const& intersect: *intersections ){
      auto const& pos = intersect.position();
      gIntersections_->SetPoint( gIntersectionsCount_++, pos.x(), pos.y() );
    }

  }

  hNIntersections_->Fill(totalIntersections);

}
// end tex::RootPart2::analyze

DEFINE_ART_MODULE(tex::RootPart2)
